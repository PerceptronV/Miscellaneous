import bs4
import requests
url='https://github.com/chinese-poetry/chinese-poetry/tree/master/json'
page=requests.get(url)
soup=bs4.BeautifulSoup(page.text,'html.parser')
soup.find_all()