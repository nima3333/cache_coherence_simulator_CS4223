import subprocess

file_name = "bodytrack"
protocol = "mesi"
cache_size = 4096
assoc_list = [1, 2, 8]
block_list = [16, 32, 64]

for assoc in assoc_list:
    for block in block_list:
        cmd = f".\multicore.exe {protocol} {file_name} {cache_size} {assoc} {block}"
        proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        o_cprog, e = proc.communicate()
        print("done !")
        fe = open(f"./{protocol}_{file_name}_{cache_size}_{assoc}_{block}.txt", "w")
        fe.write(o_cprog.decode())
        fe.close()

assoc_list2 = [1, 2, 8, 16, 32]

for assoc in assoc_list2:
    cmd = f".\multicore.exe {protocol} {file_name} {cache_size} {assoc} {128}"
    proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    o_cprog, e = proc.communicate()
    print("done !")
    fe = open(f"./{protocol}_{file_name}_{cache_size}_{assoc}_{128}.txt", "w")
    fe.write(o_cprog.decode())
    fe.close()