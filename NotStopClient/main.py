import sys
from pathlib import Path
from PySide6.QtCore import QUrl,QObject,QStringListModel
from PySide6.QtGui import QGuiApplication
from PySide6.QtQml import QQmlApplicationEngine
from PySide6.QtCore import QObject,Signal
from model import Backend
if __name__ == "__main__":
    app = QGuiApplication(sys.argv)
    #引擎导入主qml文件
    engine = QQmlApplicationEngine()
    qml_file = Path(__file__).parent / "main.qml"
    engine.load(QUrl.fromLocalFile(qml_file))

    backend = Backend()
    root = engine.rootObjects()[0]  # type: QObject
    # 找到目标对象.
    cpu_target_view = root.findChild(QObject, 'cpupage')
    cpu_target_view.setProperty("backend",backend)

    memory_target_view = root.findChild(QObject, 'memorypage')
    memory_target_view.setProperty("backend",backend)

    disk_target_view = root.findChild(QObject, 'diskpage')
    disk_target_view.setProperty("backend",backend)

    network_target_view = root.findChild(QObject, 'networkpage')
    network_target_view.setProperty("backend",backend)

    kernelmodule_target_view = root.findChild(QObject, 'kernelmodulepage')
    kernelmodule_target_view.setProperty("backend",backend)

    systeminfo_target_view = root.findChild(QObject, 'systeminfopage')
    systeminfo_target_view.setProperty("backend",backend)
    if not engine.rootObjects():
        sys.exit(-1)
    sys.exit(app.exec())
