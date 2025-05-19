import csv
import platform
import re
import subprocess

architecture = platform.machine()
block_sizes = [2**i for i in range(5, 9)]
matrix_sizes = [2**1 for i in range(5)]
with open(f"./data/eviction_{architecture}.csv", "w", newline="", buffering=1) as f:
    writer = csv.writer(f)
    csv_header = ["matrix_size", "block_size", "architecture", "I1_misses", "D1mr", "D1mw", "walltime"]
    writer.writerow(csv_header) # CSV header
    for block_size in block_sizes:
        for multiplier in [1, 2]:
            matsize = int(block_size * multiplier)
            print('matsize: ', matsize)
            print('block_size: ', block_size)
            # Run valgrind and capture stdout
            
            result = subprocess.run(
                        [
                            "valgrind",
                            "--tool=cachegrind",
                            "--cache-sim=yes",
                            f"--I1=1048576,8,{block_size}",
                            f"--D1=1048576,8,{block_size}",
                            # "--main-stacksize=33554432",
                            "--cachegrind-out-file=/dev/null", # to not save the output files
                            "./eviction", str(matsize)
                        ],
                        stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE,
                        text=True
                    )
            # parse program output
            std_err = result.stderr.strip()
            walltime = float(result.stdout.strip())

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
                matsize,
                block_size,
                architecture,
                i1,
                d1mr,
                d1mw,
                walltime
            ])
            print(f"mat={matsize},blk={block_size},I1={i1},D1mr={d1mr},D1mw={d1mw},t={walltime}")