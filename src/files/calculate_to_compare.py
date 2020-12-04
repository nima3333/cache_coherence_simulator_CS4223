import subprocess

file_name = "bodytrack"
protocols = ["moesi", "dragon", "mesi"]
cache_size = 4096
assoc_list = 32
block_list = 16

for proto in protocols:
    cmd = f".\multicore.exe {proto} {file_name} {cache_size} {assoc_list} {block_list}"
    proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    o_cprog, e = proc.communicate()
    print("done !")
    fe = open(f"./{proto}_{file_name}_{cache_size}_{assoc_list}_{block_list}.txt", "w")
    fe.write(o_cprog.decode())
    fe.close()