from dis import dis
import sys
from pathlib import Path
from PySide6.QtCore import QUrl,QObject
from PySide6.QtGui import QGuiApplication
from PySide6.QtQml import QQmlApplicationEngine,qmlRegisterType
from PySide6.QtQuick import QQuickView
from model import BaseModel
if __name__ == "__main__":
    app = QGuiApplication(sys.argv)
    qmlRegisterType(BaseModel, "BaseModel", 1, 0, "BaseModel")
    engine = QQmlApplicationEngine()
    qml_file = Path(__file__).parent / "main.qml"
    engine.load(QUrl.fromLocalFile(qml_file))

    app.exec()
    # if not engine.rootObjects():
    #     sys.exit(-1)
    # sys.exit(app.exec())