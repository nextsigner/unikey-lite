#include "ul.h"

UL::UL(QObject *parent) : QObject(parent)
{

}

void UL::cd(const QString &path)
{
    QDir dir; // QDir con el directorio de trabajo actual

    // Verifica si la ruta existe y es un directorio
    QDir targetDir(path);
    if (!targetDir.exists() || !targetDir.isReadable()) { // También verificamos si es legible
        qWarning() << "No se puede cambiar al directorio:" << path << "- El directorio no existe o no es accesible.";
        //emit currentDirectoryChanged(path, false);
        return;
    }

    // Intenta cambiar el directorio de trabajo actual
    if (dir.setCurrent(path)) {
        //qDebug() << "Directorio de trabajo cambiado a:" << dir.currentPath();
        //emit currentDirectoryChanged(dir.currentPath(), true);
    } else {
        qWarning() << "Error al cambiar el directorio de trabajo a:" << path;
        //emit currentDirectoryChanged(path, false);
    }
}
bool UL::deleteFolder(const QString &path)
{
    QDir dir(path);

    // Verifica si el directorio existe
    if (!dir.exists()) {
        qWarning() << "El directorio no existe:" << path;
        //emit directoryRemoved(path, false); // Emite la señal con fallo
        return false;
    }

    // Intenta eliminar el directorio y su contenido recursivamente
    // (Esto funciona tanto en Linux como en Windows)
    if (dir.removeRecursively()) {
        qDebug() << "Directorio eliminado exitosamente:" << path;
        //emit directoryRemoved(path, true); // Emite la señal con éxito
        return true;
    } else {
        qWarning() << "Error al eliminar el directorio:" << path;
        // Puedes agregar más detalles sobre el error si es necesario
        // QDirF:Recursively() puede fallar por permisos, archivos abiertos, etc.
        //emit directoryRemoved(path, false); // Emite la señal con fallo
        return false;
    }
}

QString UL::currentFolderPath()
{
    QString currentPath = QDir::currentPath(); // Obtiene el directorio de trabajo actual

    //qDebug() << "Directorio de trabajo actual:" << currentPath;
    //emit currentFolderPathRetrieved(currentPath); // Emite la señal con la ruta (opcional)

    return currentPath;
}

QString UL::currentFolderName()
{
    QDir f(QDir::currentPath());
    return f.dirName();
}
bool UL::deleteFile(QByteArray f)
{
    QFile arch(f);
    return arch.remove();
}

bool UL::setFile(QByteArray fileName, QByteArray fileData)
{
    return setFile(fileName, fileData, "UTF-8");
}

bool UL::setFile(QByteArray fileName, QByteArray fileData, QByteArray codec)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        lba="";
        lba.append("Cannot open file for writing: ");
        lba.append(file.errorString().toUtf8());
        //u.log(lba);
        return false;
    }
    QTextStream out(&file);
    out.setCodec(codec);
    out << fileData;
    file.close();
    return true;
}

QString UL::getFile(QByteArray n)
{
    QString r;
    QFile file(n);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return "error";
    }
    return file.readAll();
}

bool UL::folderExist(const QString &path)
{
    QDir dir(path);
    bool exists = dir.exists(); // <-- Aquí es donde se verifica la existencia

    if (exists) {
        log("La carpeta existe:");
    } else {
        log("La carpeta NO existe, creando con mkdir()...");
    }
    //emit folderExistenceChecked(path, exists); // Emite la señal con el resultado (opcional)
    return exists;
}
bool UL::fileExist(QByteArray fileName)
{
    QFile a(fileName);
    if(!a.exists()){
        QFile a2(QString(fileName).replace("\"", ""));
        if(a2.exists()){
            return a2.exists();
        }
    }
    return a.exists();
}


QList<QString> UL::getFileList(QByteArray folder, const QStringList types)
{
    QList<QString> list;

    //QDir directory("/media/ns/WD/vnRicardo");
    QDir directory(folder);
    QStringList images = directory.entryList(types,QDir::Files);
    foreach(QString filename, images) {
    //do whatever you need to do
        list.append(filename);
    }
    return list;
}


bool UL::mkdir(const QString &path)
{
    QDir dir(path);

    // Verifica si el directorio ya existe
    if (dir.exists()) {
        qWarning() << "El directorio ya existe:" << path;
        //emit directoryCreated(path, false); // Emite la señal con fallo (ya existe)
        return false;
    }

    // Intenta crear el directorio.
    // QDir::mkpath() es preferible a QDir::mkdir() porque crea todos los directorios
    // intermedios necesarios si no existen (similar a 'mkdir -p' en Linux).
    if (dir.mkpath(path)) {
        qDebug() << "Directorio creado exitosamente:" << path;
        //emit directoryCreated(path, true); // Emite la señal con éxito
        return true;
    } else {
        qWarning() << "Error al crear el directorio:" << path;
        // Esto podría fallar por permisos insuficientes o si la ruta es inválida.
        //emit directoryCreated(path, false); // Emite la señal con fallo
        return false;
    }
}

bool UL::isFolder(const QString &folder)
{
          QFileInfo archivoInfo(folder);
          return archivoInfo.isDir();
}

QList<QString> UL::getFolderFileList(const QByteArray folder)
{
    QList<QString> ret;
    QDir d(folder);
    for (int i=0;i<d.entryList().length();i++) {
        ret.append(d.entryList().at(i));
    }
    return  ret;
}

void UL::restart(const QStringList &args, const QString &newWorkingDirectory)
{
    QString applicationPath = QCoreApplication::applicationFilePath();
        QString finalWorkingDirectory = newWorkingDirectory;

        qDebug() << "Intentando reiniciar aplicación...";
        qDebug() << "Ruta de la aplicación:" << applicationPath;
        qDebug() << "Argumentos:" << args.join(" ");

        if (!newWorkingDirectory.isEmpty()) {
            QDir newDir(newWorkingDirectory);
            if (!newDir.exists() || !newDir.isReadable()) {
                qWarning() << "El nuevo directorio de trabajo especificado no existe o no es accesible:" << newWorkingDirectory;
                //emit restartAttempted(false, "El directorio de trabajo no existe o no es accesible.");
                return;
            }
            finalWorkingDirectory = QDir::cleanPath(newDir.absolutePath());
            qDebug() << "Nuevo directorio de trabajo (final):" << finalWorkingDirectory;
        } else {
            finalWorkingDirectory = QCoreApplication::applicationDirPath();
            qDebug() << "No se especificó nuevo directorio de trabajo, usando el actual:" << finalWorkingDirectory;
        }

        // Usamos QProcess::startDetached() que es no bloqueante y adecuado para esto.
        bool started = QProcess::startDetached(applicationPath, args, finalWorkingDirectory);

        if (started) {
            qDebug() << "Proceso de reinicio iniciado exitosamente.";
            //emit restartAttempted(true, "Reinicio exitoso.");

            // *** CAMBIO CLAVE AQUÍ ***
            // Retrasar el cierre de la aplicación actual para dar tiempo al nuevo proceso a independizarse.
            // Unos pocos milisegundos suelen ser suficientes.
            QTimer::singleShot(5000, QCoreApplication::instance(), &QCoreApplication::quit);
            // Opcional: Si necesitas un retardo más garantizado (aunque bloqueante para la UI en este hilo)
            // QThread::msleep(200); // Bloquea el hilo actual por 200 ms
            // QCoreApplication::quit(); // Luego, cierra la aplicación
        } else {
            qWarning() << "Fallo al iniciar el proceso de reinicio.";
            //emit restartAttempted(false, "Fallo al iniciar el proceso de reinicio.");
        }
}
void UL::restartApp()
{

#ifndef Q_OS_ANDROID
#ifndef Q_OS_IOS
    qApp->quit();
    //QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    QProcess::startDetached(qApp->arguments()[0], QStringList());
#endif
#else
    //qApp->quit();
    //QProcess::startDetached(qApp->arguments()[0], qApp->arguments());

    auto activity = QtAndroid::androidActivity();
    auto packageManager = activity.callObjectMethod("getPackageManager", "()Landroid/content/pm/PackageManager;");

    auto activityIntent = packageManager.callObjectMethod("getLaunchIntentForPackage",
                                                          "(Ljava/lang/String;)Landroid/content/Intent;",
                                                          activity.callObjectMethod("getPackageName",
                                                                                    "()Ljava/lang/String;").object());

    auto pendingIntent = QAndroidJniObject::callStaticObjectMethod("android/app/PendingIntent", "getActivity",
                                                                   "(Landroid/content/Context;ILandroid/content/Intent;I)Landroid/app/PendingIntent;",
                                                                   activity.object(), jint(0), activityIntent.object(),
                                                                   QAndroidJniObject::getStaticField<jint>("android/content/Intent",
                                                                                                           "FLAG_ACTIVITY_CLEAR_TOP"));

    auto alarmManager = activity.callObjectMethod("getSystemService",
                                                  "(Ljava/lang/String;)Ljava/lang/Object;",
                                                  QAndroidJniObject::getStaticObjectField("android/content/Context",
                                                                                          "ALARM_SERVICE",
                                                                                          "Ljava/lang/String;").object());

    alarmManager.callMethod<void>("set",
                                  "(IJLandroid/app/PendingIntent;)V",
                                  QAndroidJniObject::getStaticField<jint>("android/app/AlarmManager", "RTC"),
                                  jlong(QDateTime::currentMSecsSinceEpoch() + 1500), pendingIntent.object());

    qApp->quit();
#endif
    //emit restartingApp();
}

void UL::restartApp(QString args)
{
    qApp->quit();
    QStringList al = args.split(",");
    qDebug()<<"Restarting executable "<<qApp->applicationFilePath();
#ifdef Q_OS_LINUX
    QProcess::startDetached(qApp->applicationFilePath(), al);
#else
    QProcess::startDetached(qApp->arguments()[0], al);
#endif
}

bool UL::run(QString commandLine){
    return run(commandLine, false, 0);
}

bool UL::run(QString commandLine, bool waitingForFinished, int milliseconds)
{
#ifndef Q_OS_ANDROID
    proc = new QProcess(this);
    connect(proc, SIGNAL(readyReadStandardOutput()),this, SLOT(salidaRun()));
    connect(proc, SIGNAL(readyReadStandardError()),this, SLOT(salidaRunError()));
    proc->start(commandLine);
    //proc->start("sh",QStringList() << "-c" << "ls");
    if(waitingForFinished){
        if (!proc->waitForFinished(milliseconds)){
            qDebug() << "timeout .. ";
        }
    }
    if(proc->isOpen()){
        setRunCL(true);
        QString msg;
        msg.append("Run: ");
        msg.append(commandLine);
        setUkStd(msg);
        return true;
    }else{
        QString msg;
        msg.append("No Run: ");
        msg.append(commandLine);
        setUkStd(msg);
        setRunCL(false);
    }
#endif
    return false;
}

void UL::writeRun(QString data)
{
    proc->write(data.toUtf8());
}

bool UL::runOut(QString lineaDeComando)
{
#ifndef Q_OS_ANDROID
    proc = new QProcess(this);
    connect(proc, SIGNAL(readyReadStandardOutput()),this, SLOT(salidaRun()));
    connect(proc, SIGNAL(readyReadStandardError()),this, SLOT(salidaRunError()));
    proc->startDetached(lineaDeComando);
    if(proc->isOpen()){
        setRunCL(true);
        qInfo()<<"Ejecutando "<<lineaDeComando;
        return true;
    }else{
        qInfo()<<"No se està ejecutando "<<lineaDeComando;
        setRunCL(false);
    }
#endif
    return false;
}

void UL::salidaRun()
{
    log(proc->readAllStandardOutput());
}

void UL::salidaRunError()
{
    log(proc->readAllStandardError());
}

void UL::finalizaRun(int e)
{
    QByteArray s;
    s.append("command line finished with status ");
    s.append(QString::number(e));
    log(s);
    proc->close();
}

void UL::log(QByteArray d)
{
    log(d, false);
}

void UL::log(QByteArray d, bool htmlEscaped)
{
    QString d2;
    d2.append(d);
    /*if(!_engine->rootContext()->property("setInitString").toBool()){
        initStdString.append(d2);
        initStdString.append("\n");
    }*/
    setUkStd(d2, htmlEscaped);
}

void UL::sleep(int ms)
{
    QThread::sleep(ms);
}

QString UL::getPath(int path)
{
    QString r=".";
    if(path==0){//App location Name
        r="";
        r.append(qApp->applicationDirPath());
        r.append("/");
        r.append(QFileInfo(QCoreApplication::applicationFilePath()).fileName());
    }
#ifdef Q_OS_WIN
    if(path==1){//App location
        r = qApp->applicationDirPath();
    }
#endif
#ifdef Q_OS_OSX
    if(path==1){//App location
        r = qApp->applicationDirPath();
    }
#endif
#ifdef Q_OS_LINUX
    if(path==1){//App location
        //r = QDir::currentPath();
        r = qApp->applicationDirPath();
    }
#endif
    if(path==2){//Temp location
        r = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        //qInfo()<<"getPath(2): "<<r;
    }
    if(path==3){//Doc location
#ifndef Q_OS_ANDROID
        r = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
        //r="/sdcard/Documents";
        QStringList systemEnvironment = QProcess::systemEnvironment();
        bool sdcard=false;
        for (int i = 0; i < systemEnvironment.size(); ++i) {
            QString cad;
            cad.append(systemEnvironment.at(i));
            if(cad.contains("EXTERNAL_STORAGE=/sdcard")){
                sdcard=true;
            }
        }
        qInfo()<<"uap systemEnvironment: "<<systemEnvironment;
        qInfo()<<"uap sdcard: "<<sdcard;
        if(sdcard){
            r="/sdcard/Documents";
        }else{
            r="/storage/emulated/0/Documents";
        }
        QDir doc(r);
        if(!doc.exists()){
            qInfo()<<"[1] /sdcard/Documents no exists";
            doc.mkdir(".");
            /*if(!doc.exists()){
                r="/storage/emulated/0/Documents";
                doc.setCurrent(r);
                doc.mkdir(".");
                qInfo()<<"[2] /storage/emulated/0/Documents no exists";
            }else{
                qInfo()<<"[2] /storage/emulated/0/Documents exists";
            }*/
        }else{
            qInfo()<<"[1] /sdcard/Documents exists";
        }
#endif

    }
    if(path==4){//AppData location
        r = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    }
    if(path==5){//Current Dir
        r = QDir::currentPath();
        //r="X:/Users/Default/AppData/Roaming/UniKey/zoolv4/zoolv4-main";
    }
    if(path==6){//Current Desktop
        r = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).at(0);
    }
    if(path==7){//Current Home
        r = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0);
    }
    QDir dir(r);
    if (!dir.exists()) {
        if(debugLog){
            lba="";
            lba.append("Making folder ");
            lba.append(r.toUtf8());
            log(lba);
        }
        dir.mkpath(".");
    }else{
        if(debugLog){
            lba="";
            lba.append("Folder ");
            lba.append(r.toUtf8());
            lba.append(" exist.");
        }
    }
    return r;
}

void UL::setProperty(const QString name, const QVariant &value)
{
    _engine->rootContext()->setProperty(name.toUtf8().constData(), value);
}

QVariant UL::getProperty(const QString name)
{
    return _engine->rootContext()->property(name.toUtf8());
}

int UL::getEngineObjectsCount()
{
    return _engine->rootObjects().count();
}


