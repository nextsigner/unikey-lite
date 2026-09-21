#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "ul.h"
#include "qmlclipboardadapter.h"


int main(int argc, char *argv[])
{
    QQmlApplicationEngine engine;
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);


    QGuiApplication app(argc, argv);
    UL u;
    u.setEngine(&engine);

    //-->Preset App Name
    QByteArray presetFilePath="";
    presetFilePath.append(qApp->applicationDirPath());
    presetFilePath.append("/preset");

    QString presetAppName="";
    if(u.fileExist(presetFilePath)){
        presetAppName.append(u.getFile(presetFilePath));
        presetAppName=presetAppName.replace("\n", "");
    }else{
        presetAppName.append("UniKey");    }

    qDebug()<<"Runing "<<presetAppName<<"...";
    //<--Preset App Name

    //Clipboard function for GNU/Linux, Windows and Macos
#ifndef Q_OS_ANDROID
    QmlClipboardAdapter clipboard;
#endif

    qmlRegisterType<UL>("unik.Unik", 1, 0, "Unik");
    //<--Register Types
    qmlRegisterType<UnikQProcess>("unik.UnikQProcess", 1, 0, "UnikQProcess");




    //-->Set Unik Version
    QString nv;
    QByteArray fvp;
#ifdef Q_OS_ANDROID
    fvp.append("assets:");
#else
    fvp.append(qApp->applicationDirPath());
#endif
    fvp.append("/version");
    nv = u.getFile(fvp);
    nv = QString(nv).replace("\n", "");
    app.setApplicationVersion(nv);
    //<--Set Unik Version
    app.setApplicationDisplayName(presetAppName);
    app.setApplicationName(presetAppName);
    app.setOrganizationDomain("unikode.org");

    //-->Set engine properties
    engine.rootContext()->setContextProperty("engine", &engine);
    engine.rootContext()->setContextProperty("u", &u);
    engine.rootContext()->setContextProperty("presetAppName", presetAppName);
    //<--Set engine properties


    engine.rootContext()->setContextProperty("clipboard", &clipboard);



    //-->Set Import Path
    QByteArray ip="";
    ip.append(qApp->applicationDirPath());
    ip.append("/modules");
    engine.addImportPath(ip);
    engine.addImportPath("./modules");
    engine.addImportPath("qrc:/modules");
    //<--Set Import Path

    //QDir::setCurrent(u.getPath(4));

    engine.rootContext()->setContextProperty("argtitle", presetAppName);
    for (int i = 0; i < argc; ++i) {
        QString arg;
        arg.append(argv[i]);
        if(arg.contains("-title=")){
            engine.rootContext()->setContextProperty("argTitle", arg);
        }
    }

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
