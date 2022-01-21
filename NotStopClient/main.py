from dis import dis
import sys
from pathlib import Path
from PySide6.QtCore import QUrl,QObject,qInstallMessageHandler,QStringListModel
from PySide6.QtGui import QGuiApplication
from PySide6.QtQml import QQmlApplicationEngine,qmlRegisterType
from PySide6.QtQuick import QQuickView

from LinuxTest import getDiskInfo
def log(mode, ctx, msg):
    print(ctx.file, ctx.line, ctx.function, msg, sep='\t>>\t')
if __name__ == "__main__":
    app = QGuiApplication(sys.argv)
    # qmlRegisterType(BaseModel, "BaseModel", 1, 0, "BaseModel")
    # qInstallMessageHandler(log)

    engine = QQmlApplicationEngine()
    qml_file = Path(__file__).parent / "main.qml"
    engine.load(QUrl.fromLocalFile(qml_file))
    # context = engine.rootContext()
    diskList  = getDiskInfo()
    model = QStringListModel()
    model.setStringList(diskList)
    engine.rootContext().setContextProperty("myModel", model)

    
    if not engine.rootObjects():
        sys.exit(-1)
    sys.exit(app.exec())