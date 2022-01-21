from PySide6.QtCore import QObject,Signal,QTimer
from LinuxTest import getDiskInfo
class DiskModel(QObject):
    updated = Signal(str,arguments=["time"])
    def __init__(self) -> None:
        super().__init__()
        self.timer = QTimer()
        self.timer.setInterval(100)
        self.timer.timeout.connect(self.update_time)
        self.timer.start()
        self.diskInfo = None
    def update_time(self):
        curr_time = self.diskInfo = getDiskInfo()
        self.updated.emit(curr_time)
    
    