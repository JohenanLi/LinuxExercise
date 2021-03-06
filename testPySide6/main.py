<<<<<<< HEAD
import sys
from PySide6.QtGui import QGuiApplication
from PySide6.QtQml import QQmlApplicationEngine
from PySide6.QtCore import QTimer,QObject,Signal
from time import strftime,localtime
app = QGuiApplication(sys.argv)
engine = QQmlApplicationEngine()
engine.quit.connect(app.quit)
engine.load("main.qml")
class Backend(QObject):
    updated = Signal(str,arguments=['time'])
    def __init__(self) -> None:
        super().__init__()
        self.timer = QTimer()
        self.timer.setInterval(100)
        self.timer.timeout.connect(self.update_time)
        self.timer.start()
    def update_time(self):
        curr_time = strftime("%H:%M:%S",localtime())
        print(curr_time)
        self.updated.emit(curr_time)
backend = Backend()
engine.rootObjects()[0].setProperty("backend",backend)
backend.update_time()

=======
import sys
from PySide6.QtGui import QGuiApplication
from PySide6.QtQml import QQmlApplicationEngine
from PySide6.QtCore import QTimer,QObject,Signal
from time import strftime,localtime
app = QGuiApplication(sys.argv)
engine = QQmlApplicationEngine()
engine.quit.connect(app.quit)
engine.load("main.qml")
class Backend(QObject):
    updated = Signal(str,arguments=['time'])
    def __init__(self) -> None:
        super().__init__()
        self.timer = QTimer()
        self.timer.setInterval(100)
        self.timer.timeout.connect(self.update_time)
        self.timer.start()
    def update_time(self):
        curr_time = strftime("%H:%M:%S",localtime())
        print(curr_time)
        self.updated.emit(curr_time)
backend = Backend()
engine.rootObjects()[0].setProperty("backend",backend)
backend.update_time()

>>>>>>> 33a1bbb41f799208d0f46f921d9dd0eae3fb6d4a
sys.exit(app.exec())