import urllib.request
from bs4 import BeautifulSoup

page_url = "https://www.csh.rit.edu/~vance/pages/color.html"
page = urllib.request.urlopen(page_url)
s_page = BeautifulSoup(page, 'html.parser')
rows = s_page.find_all('tr')
# Need DMC code and their corresponding hex values
del rows[0]

map_data = []

for row in rows:
    columns = row.find_all('td')
    map_data.append([columns[0].text, columns[5].text])

print(map_data)
