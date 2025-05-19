import csv
import platform
import re
import subprocess

architecture = platform.machine()
block_sizes = [2**i for i in range(3, 8)]
with open(f"./data/cache_blocking_{architecture}.csv", "w", newline="", buffering=1) as f:
    writer = csv.writer(f)
    csv_header = ["matrix_size", "block_size", "architecture", "I1_misses", "D1mr", "D1mw", "walltime"]
    writer.writerow(csv_header) # CSV header
    for block_size in block_sizes:
        for multiplier in [1, 2, 4]:
            matsize = int(block_size * multiplier)
            # Run valgrind and capture stdout
            result = subprocess.run(
                        [
                            "valgrind",
                            "--tool=cachegrind",
                            "--cache-sim=yes",
                            "--I1=1048576,8,32",
                            "--D1=1048576,8,32",
                            "--cachegrind-out-file=/dev/null", # to not save the output files
                            "./dgemm_blk", str(matsize), str(block_size)
                        ],
                        stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE,
                        text=True
                    )
            # parse program output
            prog_out = result.stdout.strip()       # "block_size,matsize,walltime"
            _, _, walltime = prog_out.split(",")
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
                matsize,
                block_size,
                architecture,
                i1,
                d1mr,
                d1mw,
                walltime
            ])
            print(f"mat={matsize},blk={block_size},I1={i1},D1mr={d1mr},D1mw={d1mw},t={walltime}")