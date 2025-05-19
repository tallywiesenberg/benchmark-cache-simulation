import csv
import platform
import re
import subprocess

architecture = platform.machine()
block_sizes = [2**i for i in range(5, 10)]
with open(f"./data/sort_{architecture}.csv", "w", newline="", buffering=1) as f:
    writer = csv.writer(f)
    csv_header = ["array_length", "block_size", "architecture", "I1_misses", "D1mr", "D1mw", "walltime"]
    writer.writerow(csv_header) # CSV header
    for block_size in block_sizes:
        for multiplier in [1, 2, 4]:
            arr_length = int(block_size * multiplier)
            print("arr length: ", arr_length)
            print("blocksize: ", block_size)
            # Run valgrind and capture stdout
            result = subprocess.run(
                        [
                            "valgrind",
                            "--tool=cachegrind",
                            "--cache-sim=yes",
                            f"--I1=1048576,8,{block_size}",
                            f"--D1=1048576,8,{block_size}",
                            "--cachegrind-out-file=/dev/null", # to not save the output files
                            "./sort", str(arr_length)
                        ],
                        stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE,
                        text=True
                    )
            # parse program output
            walltime = result.stdout.strip()
            walltime = float(walltime)

            # parse valgrind output
            vg = result.stderr
            i1_match = re.search(r"I1\s+misses:\s*([\d,]+)", vg)
            d1_match = re.search(
                r"D1\s+misses:\s*[\d,]+\s*\(\s*([\d,]+)\s+rd\s*\+\s*([\d,]+)\s+wr\)",
                result.stderr
            )
            i1   = int(i1_match.group(1).replace(",", "")) if i1_match else None
            d1mr = int(d1_match.group(1).replace(",", "")) if d1_match else None
            d1mw = int(d1_match.group(2).replace(",", "")) if d1_match else None

            writer.writerow([
                arr_length,
                block_size,
                architecture,
                i1,
                d1mr,
                d1mw,
                walltime
            ])
            print(f"arr_len={arr_length},blk={block_size},I1={i1},D1mr={d1mr},D1mw={d1mw},t={walltime}")