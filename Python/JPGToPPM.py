import os

ppm_files = [f for f in os.listdir("./Symbols") if ".ppm" in f]

print("Files being processed: " + str(ppm_files))

for ppm in ppm_files:
    with open("./Symbols/" + ppm, "r") as f:
        print("Handling file ./Symbols/" +  ppm)
        ppm_content = f.readlines()
        for i in range(3, len(ppm_content)):
            ppm_content[i] = ppm_content[i][0:len(ppm_content[i]) - 2] + ppm_content[i][len(ppm_content[i]) - 1]

        new_ppm_content = "".join(ppm_content)
    with open("./Symbols/" + ppm, "w") as f:
        f.write(new_ppm_content)

