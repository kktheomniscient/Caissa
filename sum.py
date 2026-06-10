import keyboard
import math

def digit_sum_listener():
    total = 0.0
    segment_total = 0.0
    print("Start typing digits (0–9). Press Shift to show segment sum. Press Enter to show total and reset.")

    while True:
        key = keyboard.read_event()

        if key.event_type == keyboard.KEY_DOWN:
            if key.name == 'enter':
                print(f"\nFinal total: {math.ceil(total)}")
                total = 0
                segment_total = 0

            elif key.name == 'shift':
                print(f"Segment sum since last Shift: {segment_total}")
                segment_total = 0  # reset segment sum

            elif key.name.isdigit():
                num = 0.5 if key.name == '0' else int(key.name)
                total += num
                segment_total += num
                print(f"+ {num} → Segment: {segment_total}, Total: {total}")

            else:
                print(f"Ignored: {key.name}")

digit_sum_listener()
