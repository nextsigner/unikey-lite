import QtQuick 2.0
import ZipManager 4.0

Rectangle{
    id: r
    color: apps.backgroundColor
    //anchors.fill: parent
    width: parent.width
    height: colAll.height
    property alias zm: zipManager
    property bool dev: false
    property string title: ''
    MouseArea{
        anchors.fill: parent
    }
    Column{
        id: colAll
        width: r.width-app.fs*4
        spacing: app.fs
        anchors.centerIn: parent
        Text {
            text: '<h2>'+r.title+'</h2>'
            font.pixelSize: app.fs
            color: apps.fontColor
            anchors.horizontalCenter: parent.horizontalCenter
            visible: r.title!==''
        }
        ZipManager{
            id: zipManager
            width: parent.width
            curlPath: Qt.platform.os==='windows'?'"'+u.getPath(1).replace(/\"/g, '')+'/curl-8.14.1_2-win64-mingw/bin/curl.exe"':'curl'
            app7ZipPath: Qt.platform.os==='windows'?'"'+u.getPath(1).replace(/\"/g, '')+'/7-Zip32/7z.exe"':'7z'
            uFolder: '"'+u.getPath(3)+'"'
            onCPorcChanged:{
                if(cPorc>=0.01){
                    r.visible=true
                }
            }
            onLog: u.log(data)
            onDownloadFinished: {
                //Retorna: downloadFinished(string url, string folderPath, string zipFileName)
                u.log('Se descargó: '+zipFileName)
                u.log('Origen: '+url)
                u.log('Destino: '+folderPath)
            }
            property string d1: ''
            property string d2: ''
            property string d3: ''
            onUnzipFinished: {
                if(url)d1=url
                if(folderPath && folderPath!=='')d2=folderPath
                if(zipFileName)d3=zipFileName
                //Retorna: unzipFinished(string url, string folderPath, string zipFileName)
                //u.log('Se ha descomprimido: '+zipFileName)
                r.visible=false
                //u.log('zipFileName: '+zipFileName)
                //u.log('zipFileName url: '+url)
                //u.log('zipFileName folderPath: '+folderPath)
                let mainPath='"'+d2.replace(/\"/g, '')+'/'+d3.replace('.zip', '-main').replace(/\"/g, '')+'"'
                let aname=(''+presetAppName).toLowerCase()
                let unikeyCfgPath=''+u.getPath(4)+'/'+aname+'.cfg'
                let j={}
                j.args={}
                j.args['folder']=mainPath
                j.args['dev']=false
                j.args['dep']=false
                u.setFile(unikeyCfgPath, JSON.stringify(j, null, 2))
                u.clearComponentCache()
                let args=[]
                args.push('-folder='+""+mainPath.replace(/\"/g, ''))
                u.restart(args, ""+mainPath.replace(/\"/g, ''))
                app.close()
            }
            onResponseRepExist:{
                if(res.indexOf('404')>=0){
                    //tiGitRep.color='red'
                    u.log('El repositorio ['+url+'] no existe.')
                }else{
                    //tiGitRep.color=apps.fontColor
                    u.log('El repositorio ['+url+'] está disponible en internet.')
                    u.log('Para probarlo presiona ENTER')
                    u.log('Para instalarlo presiona Ctrl+ENTER')
                }
            }
            onResponseRepVersion:{
                procRRV(res, url, tipo)
            }
            //
        }
    }
    Component.onCompleted: {
        //let ms=new Date(Date.now()).getTime()
        //app.j.getRD('https://github.com/nextsigner/zoolv4/releases/latest?r='+ms, setZoolLastVersion)
        //r.visible=false
        //checkRemoteVersion()
    }
    function checkRemoteVersion(){
        let jsonNot={}
        jsonNot.id='winuptate'
        jsonNot.text='Buscando actualización...'
        zpn.addNot(jsonNot, true, 20000)

        let url='https://github.com/nextsigner/zoolv4'
        let c=''
        let nUrl=url.replace('https://github.com', 'https://raw.githubusercontent.com')
        c=''+zipManager.curlPath+' -s '+nUrl+'/main/version'
        let cmd=c

        let onLogDataCode='zmu.procRRV(logData, \''+url+'\', \'update\')'
        let onFinishedCode='//Nada\n'
        let onCompleteCode='//Nada\n'
        app.j.runUqp(r, 'uqpRepVersion', cmd, onLogDataCode, onFinishedCode, onCompleteCode)
    }
    function log(t){
        zpn.log(t)
    }
    function procRRV(res, url, tipo){
        //u.log('onResponseRepVersion res: '+res)
        //u.log('onResponseRepVersion url: '+url)
        //u.log('onResponseRepVersion tipo: '+tipo)
        let nCtx=''
        let nRes=res.replace('\n', '')
        let version=''
        version='update'
        if(nRes.split('.').length>=3){
            nCtx=nRes+'_'+url+'_'+tipo
            version='update_'+nRes
            u.log('El repositorio '+url+' tiene disponible la versión '+nRes)
            zipManager.version=version
            //zpn.log('zipManager.folderRoot:'+zipManager.folderRoot)
            /*zipManager.isProbe=false
            zipManager.resetApp=false
            zipManager.setCfg=false
            //zipManager.launch=false
            zipManager.download(tiGitRep.text)*/
            let jsonNot={}
            jsonNot.id='winuptate'
            let v1=nRes
            v1=v1.replace(/ /g, '').replace(/\n/g, '')
            let v2=app.version
            v2=v2.replace(/ /g, '').replace(/\n/g, '')
            if(v1===v2){
                //if(false){
                jsonNot.text='Está utilizando la versión más actualizada de Zool.\n\nVersión actual: '+v2+'\nÚltima versión disponible: '+v1+''
                zpn.addNot(jsonNot, true, 20000)
            }else{
                jsonNot.text='Hay una nueva versión de Zool disponible\n\nVersión actual: '+v2+'\nVersión disponible: '+v1+''
                //jsonNot.url='https://github.com/nextsigner/zoolv4/releases/latest'
                //jsonNot.bot1Text='Ver Repositorio GitHub'
                let c='import QtQuick 2.0\n'
                c+='Item{\n'
                c+='    Component.onCompleted:{\n'
                c+='          zmu.visible=true\n'
                c+='          let url = "https://github.com/nextsigner/zoolv4"\n'
                c+='          zmu.zm.version="'+v1+'"\n'
                c+='          zmu.zm.isProbe=false\n'
                c+='          zmu.zm.resetApp=false\n'
                c+='          zmu.zm.setCfg=false\n'
                c+='          zmu.zm.download(url)\n'
                c+='    }\n'
                c+='}\n'
                jsonNot.qml=c
                jsonNot.bot2Text='Actualizar ahora'
                zpn.addNot(jsonNot, true, 20000)
            }
        }else{
            u.log('El repositorio '+url+' NO tiene un archivo "version" disponible.')
        }
    }
}
