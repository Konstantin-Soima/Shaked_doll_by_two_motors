## Input Connector (G‑1‑2‑3)

The G‑1‑2‑3 block provides three digital inputs used as a 3‑bit mode selector.  
Each input is internally pulled up. Activating an input means connecting it to **G (ground)**.

### Pin Mapping

| Terminal | Wire Color | ESP32‑S3 Pin | Code Name | Function |
|----------|------------|--------------|-----------|----------|
| **G**    | Brown      | GND          | —         | Common ground for all inputs |
| **1**    | Yellow     | GPIO 6       | `PIN_A`   | Mode bit 0 |
| **2**    | Blue       | GPIO 7       | `PIN_B`   | Mode bit 1 |
| **3**    | Red        | GPIO 33      | `PIN_C`   | Mode bit 2 |

### Mode Logic

Inputs are read as:

mode = 7
- digitalRead(PIN_A)
- (digitalRead(PIN_B) << 1)
- (digitalRead(PIN_C) << 2);
Since inputs use `INPUT_PULLUP`:
- Open = `1`
- Connected to G = `0`

### Mode Table

| Mode | Binary (C B A) | Description | Triggered Function |
|------|----------------|-------------|---------------------|
| **0** | 000 | All inputs grounded | `move_func_down()` |
| **1** | 001 | Only A open | `move_func_up()` |
| **2** | 010 | Only B open | `move_func_fast()` |
| **3** | 011 | A + B open | `move_func_slow()` |
| **4** | 100 | Only C open | `move_func_up_down()` |
| **5–7** | 101–111 | Not used | No action |

### Summary

Different combinations of inputs select different servo motion patterns.  
A change in input state triggers the corresponding movement once.
