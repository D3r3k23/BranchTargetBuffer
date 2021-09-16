from enum import Enum
from dataclasses import dataclass
import os.path
import csv

TRACE_NAME = 'Li_int'
SM = 'Class_SM'

BTB_SIZE = 1024
PRINT_INACTIVE_ENTRIES = True

def main():
    trace = load_trace_file(f'{TRACE_NAME}.txt')
    if trace:
        btb = BTB()
        btb.process_trace(trace)
        btb.print_to_file(f'BTB.{TRACE_NAME}.{SM}.csv')

def load_trace_file(fn):
    try:
        with open(os.path.join('data', fn), 'r') as f:
            lines = f.readlines()
    except IOError:
        trace = None
        print(f'Error: Could not open file: {fn}')
        print()
    else:
        # Store hex numbers, strip whitespace, skip last line
        trace = [ int(line.strip(), 16) for line in lines if line != '\n' ]

        print(f'Traces from: {fn} loaded.')
        print(f'Number of traces: {len(trace)}')
        print()
    
    return trace

class State(Enum):
    S0 = 0b00
    S1 = 0b01
    S2 = 0b10
    S3 = 0b11

    def __str__(self):
        return format(self.value, '02b')

class Class_SM:
    def __init__(self):
        self.reset()

    def reset(self):
        self.state = State.S0
        return self.state

    def taken(self):
        return self.state == State.S0 or self.state == State.S1
    
    def go_to_next_state(self, taken):
        if self.state == State.S0: self.state = State.S0 if taken else State.S1; return self.state
        if self.state == State.S1: self.state = State.S0 if taken else State.S2; return self.state
        if self.state == State.S2: self.state = State.S1 if taken else State.S3; return self.state
        if self.state == State.S3: self.state = State.S2 if taken else State.S3; return self.state
 
    @classmethod
    def __str__(cls):
        return 'Class_SM'

class SM_B:
    def __init__(self):
        self.reset()

    def reset(self):
        self.state = State.S1
        return self.state

    def taken(self):
        return self.state == State.S0 or self.state == State.S1
    
    def go_to_next_state(self, taken):
        if self.state == State.S0: self.state = State.S0 if taken else State.S1
        if self.state == State.S1: self.state = State.S0 if taken else State.S2
        if self.state == State.S2: self.state = State.S0 if taken else State.S3
        if self.state == State.S3: self.state = State.S2 if taken else State.S3
        return self.state
    
    @classmethod
    def __str__(cls):
        return 'SM_B'

SM_c = Class_SM if SM == 'Class_SM' else SM_B

@dataclass
class Stats:
    IC: int = 0
    hits: int = 0
    misses: int = 0
    right: int = 0
    wrong: int = 0
    taken: int = 0
    collisions: int = 0
    wrong_addr: int = 0

    def __str__(self):
        return (
            f'IC:         {self.IC}\n'
            f'Hits:       {self.hits}\n'
            f'Misses:     {self.misses}\n'
            f'Right:      {self.right}\n'
            f'Wrong:      {self.wrong}\n'
            f'Taken:      {self.taken}\n'
            f'Collisions: {self.collisions}\n'
            f'Wrong addr: {self.wrong_addr}\n'
            '\n'
            f'Hit rate:   {100 * (self.hits / (self.hits + self.misses)):.2f}%\n'
            f'Accuracy:   {100 * (self.right / self.hits):.2f}%\n'
            f'Wrong addr: {100 * (self.wrong_addr / self.wrong):.2f}%'
        )

class BTB:
    @dataclass
    class Entry:
        PC: int
        target: int
        prediction: Class_SM
        busy: bool

    def __init__(self):
        self.table = [ BTB.Entry(0, 0, SM_c(), False) for i in range(BTB_SIZE) ]
        self.stats = Stats()

    @staticmethod
    def address_to_index(address):
        return (address >> 2) & 0x3FF
    
    def add_entry(self, index, PC, nextPC):
        if index < BTB_SIZE:
            entry = self.table[index]
            
            if not entry.busy:
                entry.PC = PC
                entry.target = nextPC
                entry.prediction.reset()
                entry.busy = True

    def process_trace(self, trace):
        for i, address in enumerate(trace[:-1]):
            self.process_instruction(trace, next(trace))
        else:
            self.process_last_instruction(next(trace))

        print('Stats:')
        print()
        print(self.stats)
        print()

    def process_instruction(self, PC, nextPC):
        self.stats.IC += 1
        index = BTB.address_to_index(PC)
        entry = self.table[index]
        taken = nextPC != PC + 4

        if taken:
            self.stats.taken += 1
        
        if entry.busy and entry.PC == PC: # BTB hit
            self.stats.hits += 1

            if entry.prediction.taken() == taken: # Right prediction
                if taken and nextPC != entry.target: # Wrong address
                    self.stats.wrong += 1
                    self.stats.wrong_addr += 1

                    entry.target = nextPC
                else:
                    self.stats.right += 1

            else: # Wrong prediction
                self.stats.wrong += 1
            
            entry.prediction.go_to_next_state(taken)

        elif taken: # BTB miss
            self.stats.misses += 1

            if entry.busy: # Collision
                self.stats.collisions += 1
                entry.busy = False

            self.add_entry(index, PC, nextPC)

    def process_last_instruction(self, PC):
        self.stats.IC += 1
        index = BTB.address_to_index(PC)
        entry = self.table[index]

        if entry.busy and entry.PC == PC:
            self.stats.hits += 1

    def print_to_file(self, fn):
        rows = [
            {
                'Index': format(index, '>4'),
                'PC': format(entry.PC, '08X'),
                'Target': format(entry.target, '08X'),
                'State Machine': str(entry.prediction.state),
                'Prediction': 'Taken' if entry.prediction.taken() else 'NT   ',
                'Busy': str(entry.busy).lower()
            }
            for index, entry in enumerate(self.table) if entry.busy or PRINT_INACTIVE_ENTRIES
        ]

        try:
            with open(os.path.join('Python', 'output', fn), 'w', newline='') as f:
                writer = csv.DictWriter(f, rows[0].keys())
                writer.writeheader()
                writer.writerows(rows)
                print(f'BTB contents printed to: {fn}')
        except IOError:
            print(f'Error: Could not open: {fn}')

if __name__ == '__main__':
    main()
