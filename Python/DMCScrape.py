import urllib.request
from bs4 import BeautifulSoup

# Get DMC floss table

page_url = "https://www.csh.rit.edu/~vance/pages/color.html"
page = urllib.request.urlopen(page_url)
s_page = BeautifulSoup(page, 'html.parser')
rows = s_page.find_all('tr')

# Remove title table row
del rows[0]

map_data = []

# For each row, get the DMC code and RGB
for row in rows:
    columns = row.find_all('td')
    map_data.append([columns[0].text, columns[5].text])

# Open target file to generate
target_file = open("DMC.h", "w")

# Print macros
target_file.write("#ifndef DMC_H\n#define DMC_H\n\n#include \"Map.h\"\n\n")

# Print pairs
number_of_pairs = len(map_data)

target_file.write("#define FLOSS_COUNT " + str(number_of_pairs) + "\n\n")
target_file.write("static struct kv_pair DMC_flosses[FLOSS_COUNT] = \n{\n")

for i in range(number_of_pairs):
    target_file.write("kv_pair{0x" + map_data[i][1] + ", " + map_data[i][0] + "}")
    if i != number_of_pairs - 1:
        target_file.write(",")
    target_file.write("\n")

target_file.write("};\n")
target_file.write("struct rb_tree* create_DMC_floss_map();\n\n")
target_file.write("#endif\n")

target_file.close()
print(number_of_pairs)
