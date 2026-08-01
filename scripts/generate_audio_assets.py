"""Generate small, deterministic retro arcade audio assets for TankWar."""

from __future__ import annotations

import math
import random
import struct
import wave
from pathlib import Path


SAMPLE_RATE = 44_100
ROOT = Path(__file__).resolve().parents[1]
MUSIC_DIR = ROOT / "assets" / "audio" / "music"
SFX_DIR = ROOT / "assets" / "audio" / "sfx"


def clamp(value: float) -> float:
    return max(-1.0, min(1.0, value))


def write_wav(path: Path, samples: list[tuple[float, float]]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    pcm = bytearray()
    for left, right in samples:
        pcm.extend(struct.pack("<hh", int(clamp(left) * 32767), int(clamp(right) * 32767)))

    with wave.open(str(path), "wb") as output:
        output.setnchannels(2)
        output.setsampwidth(2)
        output.setframerate(SAMPLE_RATE)
        output.writeframes(pcm)


def envelope(position: float, duration: float, attack: float = 0.01, release: float = 0.08) -> float:
    if position < 0.0 or position >= duration:
        return 0.0
    attack_level = 1.0 if attack <= 0.0 else min(1.0, position / attack)
    release_start = max(0.0, duration - release)
    release_level = 1.0 if position < release_start else max(0.0, (duration - position) / release)
    return attack_level * release_level


def square_wave(frequency: float, time: float, duty: float = 0.5) -> float:
    return 1.0 if (time * frequency) % 1.0 < duty else -1.0


def triangle_wave(frequency: float, time: float) -> float:
    phase = (time * frequency) % 1.0
    return 4.0 * abs(phase - 0.5) - 1.0


def add_note(buffer: list[float], start: float, duration: float, frequency: float,
             amplitude: float, wave_fn, attack: float = 0.01, release: float = 0.08) -> None:
    first = max(0, int(start * SAMPLE_RATE))
    last = min(len(buffer), int((start + duration) * SAMPLE_RATE))
    for index in range(first, last):
        position = index / SAMPLE_RATE - start
        value = wave_fn(frequency, position)
        buffer[index] += value * amplitude * envelope(position, duration, attack, release)


def pan(sample: float, amount: float) -> tuple[float, float]:
    left = sample * (1.0 - max(0.0, amount))
    right = sample * (1.0 + min(0.0, amount))
    return left, right


def make_music(duration: float, bpm: float, melody: list[float], bass: list[float], seed: int) -> list[tuple[float, float]]:
    frame_count = int(duration * SAMPLE_RATE)
    melody_buffer = [0.0] * frame_count
    bass_buffer = [0.0] * frame_count
    beat = 60.0 / bpm
    eighth = beat / 2.0

    for index, frequency in enumerate(melody):
        if frequency > 0.0:
            add_note(melody_buffer, index * eighth, eighth * 0.86, frequency, 0.19,
                     lambda freq, time: square_wave(freq, time, 0.32), attack=0.004, release=0.045)

    for index, frequency in enumerate(bass):
        if frequency > 0.0:
            add_note(bass_buffer, index * beat, beat * 0.82, frequency, 0.20,
                     triangle_wave, attack=0.008, release=0.08)

    noise = random.Random(seed)
    output: list[tuple[float, float]] = []
    for index in range(frame_count):
        time = index / SAMPLE_RATE
        beat_position = time / beat
        percussion = 0.0
        sixteenth = beat / 4.0
        local = time % sixteenth
        if local < 0.035:
            kick = math.exp(-local * 75.0) * (0.24 if int(beat_position) % 4 == 0 else 0.13)
            percussion += kick * math.sin(2.0 * math.pi * (85.0 - 45.0 * min(1.0, local / 0.035)) * local)
        if int(beat_position * 2.0) % 2 == 1 and local < 0.022:
            percussion += (noise.random() * 2.0 - 1.0) * math.exp(-local * 130.0) * 0.06

        lead = melody_buffer[index]
        low = bass_buffer[index]
        wobble = math.sin(2.0 * math.pi * 5.0 * time) * 0.012
        left = lead + low + percussion + wobble
        right = lead * 0.92 + low * 1.02 + percussion + wobble
        output.append((clamp(left), clamp(right)))
    return output


def make_tone(duration: float, frequencies: list[tuple[float, float, float]], noise_amount: float = 0.0,
              seed: int = 0) -> list[tuple[float, float]]:
    frame_count = int(duration * SAMPLE_RATE)
    random_source = random.Random(seed)
    output: list[tuple[float, float]] = []
    for index in range(frame_count):
        time = index / SAMPLE_RATE
        value = 0.0
        for start, frequency, amplitude in frequencies:
            if time >= start:
                value += square_wave(frequency, time - start, 0.5) * amplitude
        value += (random_source.random() * 2.0 - 1.0) * noise_amount
        fade = min(1.0, time / 0.008) * min(1.0, (duration - time) / 0.06)
        output.append((clamp(value * fade), clamp(value * fade * 0.92)))
    return output


def make_shot() -> list[tuple[float, float]]:
    duration = 0.22
    frame_count = int(duration * SAMPLE_RATE)
    output: list[tuple[float, float]] = []
    random_source = random.Random(11)
    for index in range(frame_count):
        time = index / SAMPLE_RATE
        decay = math.exp(-time * 28.0)
        sweep = 160.0 - 100.0 * min(1.0, time / duration)
        value = math.sin(2.0 * math.pi * sweep * time) * 0.48 * decay
        value += (random_source.random() * 2.0 - 1.0) * 0.28 * math.exp(-time * 40.0)
        output.append((clamp(value), clamp(value * 0.92)))
    return output


def make_explosion() -> list[tuple[float, float]]:
    duration = 0.62
    frame_count = int(duration * SAMPLE_RATE)
    output: list[tuple[float, float]] = []
    random_source = random.Random(23)
    for index in range(frame_count):
        time = index / SAMPLE_RATE
        low = math.sin(2.0 * math.pi * (72.0 - 45.0 * time) * time) * math.exp(-time * 5.2)
        crackle = (random_source.random() * 2.0 - 1.0) * math.exp(-time * 7.5)
        value = low * 0.62 + crackle * 0.52
        output.append((clamp(value), clamp(value * 0.88)))
    return output


def make_break() -> list[tuple[float, float]]:
    duration = 0.28
    frame_count = int(duration * SAMPLE_RATE)
    output: list[tuple[float, float]] = []
    random_source = random.Random(31)
    for index in range(frame_count):
        time = index / SAMPLE_RATE
        value = (random_source.random() * 2.0 - 1.0) * math.exp(-time * 16.0) * 0.55
        value += triangle_wave(220.0, time) * math.exp(-time * 19.0) * 0.18
        output.append((clamp(value), clamp(value * 0.9)))
    return output


def main() -> None:
    # Frequencies are written explicitly to keep the generated loops deterministic and self-contained.
    menu_melody = [440, 0, 523.25, 0, 659.25, 0, 523.25, 0,
                   392, 0, 493.88, 0, 587.33, 0, 493.88, 0,
                   440, 0, 523.25, 0, 659.25, 0, 783.99, 0,
                   659.25, 0, 587.33, 0, 523.25, 0, 493.88, 0]
    menu_bass = [110, 110, 130.81, 130.81, 146.83, 146.83, 130.81, 130.81,
                 98, 98, 123.47, 123.47, 146.83, 146.83, 123.47, 123.47]
    battle_melody = [293.66, 293.66, 349.23, 0, 440, 349.23, 293.66, 0,
                     261.63, 261.63, 329.63, 0, 392, 329.63, 261.63, 0,
                     293.66, 293.66, 349.23, 0, 440, 493.88, 440, 349.23,
                     329.63, 329.63, 392, 0, 493.88, 440, 392, 0] * 2
    battle_bass = [73.42, 73.42, 87.31, 87.31, 98.0, 98.0, 87.31, 87.31,
                   65.41, 65.41, 82.41, 82.41, 98.0, 98.0, 82.41, 82.41] * 2

    write_wav(MUSIC_DIR / "menu_theme.wav", make_music(60.0 / 112.0 * 16.0, 112.0, menu_melody, menu_bass, 101))
    write_wav(MUSIC_DIR / "battle_theme.wav", make_music(60.0 / 140.0 * 28.0, 140.0, battle_melody, battle_bass, 202))

    write_wav(SFX_DIR / "menu_move.wav", make_tone(0.07, [(0.0, 740.0, 0.24)]))
    write_wav(SFX_DIR / "menu_confirm.wav", make_tone(0.16, [(0.0, 523.25, 0.22), (0.07, 783.99, 0.22)]))
    write_wav(SFX_DIR / "tank_shoot.wav", make_shot())
    write_wav(SFX_DIR / "tank_hit.wav", make_tone(0.17, [(0.0, 180.0, 0.32), (0.04, 95.0, 0.22)], noise_amount=0.16, seed=17))
    write_wav(SFX_DIR / "tank_explosion.wav", make_explosion())
    write_wav(SFX_DIR / "powerup_collect.wav", make_tone(0.34, [(0.0, 523.25, 0.18), (0.08, 659.25, 0.18), (0.16, 783.99, 0.18), (0.24, 1046.5, 0.2)]))
    write_wav(SFX_DIR / "brick_break.wav", make_break())
    write_wav(SFX_DIR / "player_damage.wav", make_tone(0.24, [(0.0, 330.0, 0.22), (0.08, 220.0, 0.2), (0.16, 146.83, 0.18)], noise_amount=0.03, seed=41))


if __name__ == "__main__":
    main()
