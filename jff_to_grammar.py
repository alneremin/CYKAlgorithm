
import xml.etree.ElementTree as ET
import os
import sys

"""
#   Скрипт для преобразования jff формат грамматики в grammar
#   Запуск через интерпретатор: python3 jff_to_grammar.py path/to/jff
"""
if __name__ == "__main__":

    try:
        tree = ET.parse(sys.argv[1])
    except:
        print("Введите аргумент - путь до файла .jff")
        sys.exit(1)

    root = tree.getroot()

    if (root[0].text != "grammar"):
        print("invalid jff-file (not grammar)")
        sys.exit(1)
    filename = os.path.split(sys.argv[1])[1][:-3] + root[0].text

    with open(filename, "w") as f:
        for child in root[1:]:
            f.write(child.find('left').text + "->" + child.find('right').text + '\n')
    print("file saved in " + os.path.join(os.getcwd(), filename))