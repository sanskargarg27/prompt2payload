import os
import time
from PIL import Image, ImageDraw, ImageFont
from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QLabel

class Scareware(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setGeometry(0, 0, 640, 480)
        self.centralwidget = QWidget()
        self.layout = QVBoxLayout()
        self.centralwidget.setLayout(self.layout)
        self.setCentralWidget(self.centralwidget)
        
        font = ImageFont.truetype('arial.ttf', 15)
        draw = ImageDraw.Draw(Image.new('RGB', (640, 20), 'white'))

        self.label = QLabel()
        text = f'Scanning...\n{time.strftime("%Y-%m-%d %H:%M:%S")}'
        text_pos = 5, 30
        draw.text(text_pos, font, text, fill='black')

        self.layout.addWidget(self.label)
        self.show()
        
if __name__ == '__main__':
    app = QApplication([])
    scareware = Scareware()
    sys.exit(app.exec_())
