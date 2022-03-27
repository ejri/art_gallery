import pyautogui
import time
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.keys import Keys

chrome_options = Options()
chrome_options.add_experimental_option("useAutomationExtension", False)
chrome_options.add_experimental_option("excludeSwitches", ["enable-automation"])
chrome_options.add_argument("--kiosk")

driver = "/usr/lib/chromium-browser/chromedriver"
chrome_options.binary_location = "/usr/lib/chromium-browser/chromium-browser"
browser = webdriver.Chrome(options=chrome_options)

# browser.get(url_defualt)
# new_tab()
# browser.get(url_jar1)
# close_tab()

def close_tab():
    time.sleep(15)
    pyautogui.hotkey("ctrl", "w")
    print("tab closed")


def new_tab():
    time.sleep(2)
    browser.find_element_by_tag_name("body").send_keys(Keys.CONTROL + "t")
    # pyautogui.hotkey("ctrl", "t") # open a new tab with google search bar
    print("new tab")