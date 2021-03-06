 /*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusObjectPath>
#include <QDBusReply>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QColor>

#include "clickLabel.h"
#include "MacroFile.h"

//typedef void(*GAsyncReadyCallback) (GDrive *source_object,GAsyncResult *res,gpointer user_data);

//void frobnitz_result_func_mount(GMount *source_object,GAsyncResult *res,MainWindow *p_this)
//{
//    gboolean success = FALSE;
//    GError *err = nullptr;

//    success = g_mount_unmount_with_operation_finish(source_object,res,&err);
//    if(success)
//    {
//        if(g_mount_can_unmount(source_object))
//        {
//            findGMountList()->removeOne(source_object);
//            p_this->m_eject = new ejectInterface(p_this,g_volume_get_name(g_mount_get_volume(source_object)),DATADEVICE);
//            p_this->m_eject->show();
//        }
//    }
//    else
//    {
//        p_this->m_eject = new ejectInterface(p_this,g_drive_get_name(g_volume_get_drive(g_mount_get_volume(source_object))),OCCUPYDEVICE);
//        p_this->m_eject->show();
//    }

//}

void frobnitz_result_func(GDrive *source_object,GAsyncResult *res,MainWindow *p_this)
{
    gboolean success =  FALSE;
    GError *err = nullptr;
    success = g_drive_eject_with_operation_finish (source_object, res, &err);

//    qDebug()<<"oh no"<<err->message<<err->code;

    if (!err)
    {
      findGDriveList()->removeOne(source_object);
      p_this->m_eject = new ejectInterface(p_this,g_drive_get_name(source_object),NORMALDEVICE);
      p_this->m_eject->show();
    }

    else if(g_drive_can_stop(source_object) == true)
    {
        qDebug()<<"aaaaaaaaaaaaa-&& g_drive_can_stop(source_object) == true----------1";
        int volumeNum = g_list_length(g_drive_get_volumes(source_object));

        for(int eachVolume = 0 ; eachVolume < volumeNum ;eachVolume++)
        {
//            g_mount_unmount_with_operation(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(source_object),eachVolume)),
//                                           G_MOUNT_UNMOUNT_NONE,
//                                           NULL,
//                                           NULL,
//                                           GAsyncReadyCallback(frobnitz_result_func_mount),
//                                           p_this
//                        );
            p_this->flagType = 0;

            if(g_mount_can_unmount(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(source_object),eachVolume))))
            {
//                UDiskPathDis1 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0))));
                char *dataPath = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(source_object),eachVolume))));
//                QProcess::execute("pkexec umount " + QString(dataPath));
//                p_this->flagType++;
//                qDebug()<<QString(dataPath)<<"  aaaaaaaaaaaaa-------------------------------1.5";
                QProcess p;
                p.setProgram("pkexec");
                p.setArguments(QStringList()<<"umount"<<QString(dataPath));
                p.start();
                p_this->ifSucess = p.waitForFinished();
                p_this->m_eject = new ejectInterface(p_this,g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(source_object),eachVolume)),DATADEVICE);
                p_this->m_eject->show();
                findGMountList()->removeOne(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(source_object),eachVolume)));
                findGDriveList()->removeOne(source_object);
                qDebug()<<"-----"<<findGDriveList()->size()<<";;;;;;;;;;;;;;;;"<<findGMountList()->size();
                qDebug()<<"oh no"<<err->message<<err->code;
            }

        }
    }
    else
    {   qDebug()<<"oh no"<<err->message<<err->code;
        qDebug()<<"howwohohwohow";
        p_this->m_eject = new ejectInterface(p_this,g_drive_get_name(source_object),OCCUPYDEVICE);
        p_this->m_eject->show();
    }



//    else
//    {
//      qDebug()<<"oh no"<<err->message<<err->code;
//    }

//    if(findGDriveList()->size() == 0 || findGVolumeList()->size() == 0)
//    {
//        p_this->m_systray->hide();
//    }

}



void frobnitz_result_func_drive(GDrive *source_object,GAsyncResult *res,MainWindow *p_this)
{
    gboolean success =  FALSE;
    GError *err = nullptr;
    success = g_drive_start_finish (source_object, res, &err);
//    if(!err)
//    {
//        qDebug()<<"drive start sucess";
//    }
//    else
//    {
//        qDebug()<<"drive start failed"<<"-------------------"<<err->message<<err->code;
//    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
//    const QByteArray idtrans(THEME_QT_TRANS);

//    if(QGSettings::isSchemaInstalled(idtrans))
//    {
//        m_transparency_gsettings = new QGSettings(idtrans);
//    }
    installEventFilter(this);
    ui->setupUi(this);
    //框架的样式设置
    //set the style of the framework
    int hign = 0;
    interfaceHideTime = new QTimer(this);
    initTransparentState();

    QPainterPath path;
    auto rect = this->rect();
    rect.adjust(0,0,-0,-0);
    path.addRoundedRect(rect, 6, 6);
    setProperty("blurRegion", QRegion(path.toFillPolygon().toPolygon()));
    this->setStyleSheet("QWidget{border:none;border-radius:6px;}");
    this->setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
    ui->centralWidget->setObjectName("centralWidget");

    ui->centralWidget->setStyleSheet(
                "#centralWidget{"
                "width:280px;"
                "height:192px;"
                "border:1px solid rgba(255, 255, 255, 0.05);"
                "opacity:0.75;"
                "border-radius:6px;"
                "box-shadow:0px 2px 6px 0px rgba(0, 0, 0, 0.2);"
//                "margin:0px;"
//                "border-width:0px;"
//                "padding:0px;"
                "}"
                );

//    getTransparentData();


//                "background:rgba(19,19,20,0.95);"
    //to get the picture from the theme

    iconSystray = QIcon::fromTheme("media-removable-symbolic");
    vboxlayout = new QVBoxLayout();
    //hboxlayout = new QHBoxLayout();
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
#else
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
#endif

    //this->resize( QSize( 280, 192 ));
    m_systray = new QSystemTrayIcon ;
    //show();
    //m_systray->setIcon(QIcon("/usr/share/icons/ukui-icon-theme-default/22x22/devices/drive-removable-media.png"));
    m_systray->setIcon(iconSystray);
    m_systray->setToolTip(tr("usb management tool"));
    //init the srceen
    screen = qApp->primaryScreen();
    //underlying code to get the information of the usb device
    getDeviceInfo();
    connect(m_systray, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
    connect(this,&MainWindow::convertShowWindow,this,&MainWindow::onConvertShowWindow);
    ui->centralWidget->setLayout(vboxlayout);

    //    QDBusConnection::sessionBus().connect(QString(),QString("/taskbar/click"), \
    //                                          "com.ukui.panel.plugins.taskbar","sendToUkuiDEApp",this, \
    //                                          SLOT(on_clickPanelToHideInterface));

    //    QDBusConnection::sessionBus().connect(QString(), QString("/taskbar/click"),
    //                                          "com.ukui.panel.plugins.taskbar", "sendToUkuiDEApp", this,
    //                                          SLOT(on_clickPanelToHideInterface));
        QDBusConnection::sessionBus().connect(QString(), QString("/taskbar/click"), \
                                                  "com.ukui.panel.plugins.taskbar", "sendToUkuiDEApp", this, SLOT(on_clickPanelToHideInterface()));
}

MainWindow::~MainWindow()
{
    delete ui;
//    delete open_widget;
}

void MainWindow::on_clickPanelToHideInterface()
{
//    if(MainWindow::isShow == true)
//    {
//        this->hide();
//        MainWindow::isShow = false;
//    }
    if(!this->isHidden())
        this->hide();
}

void MainWindow::getDeviceInfo()
{
//callback function that to monitor the insertion and removal of the underlying equipment
    GVolumeMonitor *g_volume_monitor = g_volume_monitor_get();
    g_signal_connect (g_volume_monitor, "drive-connected", G_CALLBACK (drive_connected_callback), this);
    g_signal_connect (g_volume_monitor, "drive-disconnected", G_CALLBACK (drive_disconnected_callback), this);
    g_signal_connect (g_volume_monitor, "volume-added", G_CALLBACK (volume_added_callback), this);
    g_signal_connect (g_volume_monitor, "volume-removed", G_CALLBACK (volume_removed_callback), this);
    g_signal_connect (g_volume_monitor, "mount-added", G_CALLBACK (mount_added_callback), this);
    g_signal_connect (g_volume_monitor, "mount-removed", G_CALLBACK (mount_removed_callback), this);
//about drive
    GList *current_drive_list = g_volume_monitor_get_connected_drives(g_volume_monitor);
    while(current_drive_list)
    {
        GDrive *gdrive = (GDrive *)current_drive_list->data;
        if(g_drive_can_eject(gdrive))
        {
            if(g_volume_can_eject((GVolume *)g_list_nth_data(g_drive_get_volumes(gdrive),0)))
            {
                *findGDriveList()<<gdrive;
            }
        }
        current_drive_list = current_drive_list->next;
        //if(g_drive_can_eject)
    }
//about volume
    GList *current_volume_list = g_volume_monitor_get_volumes(g_volume_monitor);
    while(current_volume_list)
    {
        GVolume *gvolume = (GVolume *)current_volume_list->data;
        if(g_volume_can_eject(gvolume) || g_drive_can_eject(g_volume_get_drive(gvolume)))
        {
            *findGVolumeList()<<gvolume;
            g_volume_mount(gvolume,
                           G_MOUNT_MOUNT_NONE,
                           nullptr,
                           nullptr,
                           nullptr,
                           nullptr);
        }
        current_volume_list = current_volume_list->next;
    }

    GList *current_mount_list = g_volume_monitor_get_mounts(g_volume_monitor);
    while(current_mount_list)
    {
        GMount *gmount = (GMount *)current_mount_list->data;
        if(g_mount_can_eject(gmount))
        {
            *findGMountList()<<gmount;
        }
        current_mount_list = current_mount_list->next;
    }

 //determine the systray icon should be showed  or be hieded
    if(findGMountList()->size() >= 1 || findGDriveList()->size() >= 1)
    {
        m_systray->show();
    }
    if(findGDriveList()->size() == 0 || findGMountList()->size() == 0)
    {
        m_systray->hide();
    }
}

void MainWindow::onConvertShowWindow()
{
    num = 0;
    MainWindowShow();
}

//the drive-connected callback function the is triggered when the usb device is inseted

void MainWindow::drive_connected_callback(GVolumeMonitor *monitor, GDrive *drive, MainWindow *p_this)
{
    qDebug()<<"drive add";
    *findGDriveList()<<drive;
    if(findGDriveList()->size() <= 1)
    {
        p_this->m_systray->show();
    }

    p_this->triggerType = 0;
}

//the drive-disconnected callback function the is triggered when the usb device is pull out
void MainWindow::drive_disconnected_callback (GVolumeMonitor *monitor, GDrive *drive, MainWindow *p_this)
{
    qDebug()<<"drive disconnect";
    char *drive_name = g_drive_get_name(drive);
    findGDriveList()->removeOne(drive);
    p_this->hide();
    if(findGDriveList()->size() == 0)
    {
        p_this->m_systray->hide();
    }
}

//when the usb device is identified,we should mount every partition

void MainWindow::volume_added_callback(GVolumeMonitor *monitor, GVolume *volume, MainWindow *p_this)
{
    qDebug()<<"volume add";
    g_volume_mount(volume,
                   G_MOUNT_MOUNT_NONE,
                   nullptr,
                   nullptr,
                   GAsyncReadyCallback(frobnitz_result_func_volume),
                   p_this);
    //if the deveice is CD
    p_this->root = g_mount_get_default_location(g_volume_get_mount(volume));
    p_this->mount_uri = g_file_get_uri(p_this->root);
    if(p_this->mount_uri)
    {
        if(strcmp(p_this->mount_uri,"burn:///") == 0)
        {
            *findGDriveList()<<g_volume_get_drive(volume);
        }
    }
    else
    {
        char *devPath = g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);   //detective the kind of movable device
        qDebug()<<"devPath"<<devPath;
        if(g_str_has_prefix(devPath,"/dev/sr"))
        {
            *findGDriveList()<<g_volume_get_drive(volume);
        }

//        if(g_str_has_prefix(devPath,"/dev/sd"))
//        {
//            *findGMountList()<<g_volume_get_mount(volume);
//        }

    }
    g_object_unref(p_this->root);
    g_free(p_this->mount_uri);
//    g_object_unref(volume);

}


//when the U disk is pull out we should reduce all its partitions
void MainWindow::volume_removed_callback(GVolumeMonitor *monitor, GVolume *volume, MainWindow *p_this)
{
    qDebug()<<"volume removed";
    findGVolumeList()->removeOne(volume);
//    if(findGVolumeList()->size() >= 0)
//    {
//        p_this->m_systray->show();
//    }
    p_this->root = g_mount_get_default_location(g_volume_get_mount(volume));
    p_this->mount_uri = g_file_get_uri(p_this->root);
    if(p_this->mount_uri)
    {
        if(strcmp(p_this->mount_uri,"burn:///") == 0)
        {
//            *findGDriveList()<<g_volume_get_drive(volume);
            findGDriveList()->removeOne(g_volume_get_drive(volume));
        }
    }
    else
    {
        char *devPath = g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);   //detective the kind of movable device
        if(g_str_has_prefix(devPath,"/dev/sr"))
        {
            findGDriveList()->removeOne(g_volume_get_drive(volume));
        }

//        if(g_str_has_prefix(devPath,"/dev/sd"))
//        {
//            findGMountList()->removeOne(g_volume_get_mount(volume));
//        }
    }
    g_object_unref(p_this->root);
    g_free(p_this->mount_uri);
}

//when the volumes were mounted we add its mounts number
void MainWindow::mount_added_callback(GVolumeMonitor *monitor, GMount *mount, MainWindow *p_this)
{
    qDebug()<<"mount add";
    char *devPath = g_volume_get_identifier(g_mount_get_volume(mount),G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
    if(g_mount_can_eject(mount) && g_drive_can_eject(g_mount_get_drive(mount)) || g_str_has_prefix(devPath,"/dev/sd"))
    {
        qDebug()<<"yao yao wu qi";
        *findGMountList()<<mount;
    }
//    if(findGMountList())
//    *findGMountList()<<mount;
    if(findGMountList()->size() >= 1)
    {
        p_this->m_systray->show();
    }
}

//when the mountes were uninstalled we reduce mounts number
void MainWindow::mount_removed_callback(GVolumeMonitor *monitor, GMount *mount, MainWindow *p_this)
{
    qDebug()<<mount<<"mount remove1111111";
    qDebug()<<g_mount_get_volume(mount)<<"volume ---";
    qDebug()<<g_list_length(g_drive_get_volumes(g_mount_get_drive(mount)))<<"length";
    qDebug()<<g_mount_get_drive(mount)<<"gdrive ---";
    qDebug()<<"mount removed";
    findGMountList()->removeOne(mount);
    p_this->driveMountNum = 0;

    for(int i = 0; i<g_list_length(g_drive_get_volumes(g_mount_get_drive(mount)));i++)
    {
        qDebug()<<"xun huan yi jin";
        if(g_volume_get_mount((GVolume *)g_list_nth(g_drive_get_volumes(g_mount_get_drive(mount)),i)) == NULL)
        {
            p_this->driveMountNum += 1;
            qDebug()<<p_this->driveMountNum<<"driveMountNum";
        }
    }
    qDebug()<<findGDriveList()->size()<<"gdriveList size ---";

    if(findGMountList()->size() == 0)
    {
        p_this->m_systray->hide();
    }

//    Q_EMIT p_this->unloadMount();
}

//it stands that when you insert a usb device when all the  U disk partitions
void MainWindow::frobnitz_result_func_volume(GVolume *source_object,GAsyncResult *res,MainWindow *p_this)
{

    gboolean success =  FALSE;
    GError *err = nullptr;
    success = g_volume_mount_finish (source_object, res, &err);
    if(!err)
    {
        *findGVolumeList()<<source_object;
        p_this->num++;
        if (p_this->num >= g_list_length(g_drive_get_volumes(g_volume_get_drive(source_object))))
        {
            Q_EMIT p_this->convertShowWindow();     //emit a signal to trigger the MainMainShow slot
        }
    }
    else
    {
//        g_volume_mount(source_object,
//                       G_MOUNT_MOUNT_NONE,
//                       nullptr,
//                       nullptr,
//                       GAsyncReadyCallback(frobnitz_result_func_volume),
//                       nullptr);
        qDebug()<<"sorry mount failed";
    }
}

//here we begin painting the main interface
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{

//    qDebug()<<m_transparency;
//    QPalette palette = this->palette();
//    QColor color = QColor(Qt::red);
//    color.setAlphaF(m_transparency/100);
//    palette.setBrush(QPalette::Window, color);
//    this->setPalette(palette);
//    this->setAutoFillBackground(true);
//    this->setWindowOpacity(m_transparency/100);
    m_transparency = 0.00;
    this->getTransparentData();
    qDebug()<<"findGMountList.size"<<findGMountList()->size();
    qDebug()<<"findGDriveList.size"<<findGDriveList()->size();
    qDebug()<<"m_transparency"<<m_transparency;
    QString strTrans;
    strTrans =  QString::number(m_transparency, 10, 2);
    QString convertStyle = "#centralWidget{background:rgba(19,19,20," + strTrans + ");}";
    this->setStyleSheet(convertStyle);

//    if(this->interfaceHideTime != NULL)
//    {
//        delete this->interfaceHideTime;
//    }
    //int hign = 200;
    triggerType = 1;  //It represents how we open the interface
    if(ui->centralWidget != NULL)
    {
        //cancel the connection between the timeout signal and main interface hiding
        disconnect(interfaceHideTime, SIGNAL(timeout()), this, SLOT(on_Maininterface_hide()));
    }
    int num = 0;
    if ( this->vboxlayout != NULL )
    {
        QLayoutItem* item;
        while ((item = this->vboxlayout->takeAt(0)) != NULL)
        {
            delete item->widget();
            delete item;
        }
    }
    if(this->open_widget != NULL)
    {
        qDebug()<<"open_widget 11111111111";
//        open_widget->deleteLater();
    }
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::Context:
      if (this->isHidden())
      {
          //Convenient interface layout for all drives

          for(auto cacheDrive : *findGDriveList())
          {
              int singleSignal = 0;
              int cdSignal = 0;
              qDebug()<<"findGMountList:num" <<findGMountList()->size();
              qDebug()<<"findGDriveList:num" <<findGDriveList()->size();

              listVolumes = g_drive_get_volumes(cacheDrive);
              qDebug()<<"listVolumes------"<<listVolumes;
              for(vList = listVolumes; vList != NULL; vList = vList->next)
              {
                  volume = (GVolume *)vList->data;
                  if(g_volume_get_mount(volume) != NULL)
                  {
                      root = g_mount_get_default_location(g_volume_get_mount(volume));
                      mount_uri = g_file_get_uri(root);
                      if(g_str_has_prefix(mount_uri,"file:///"))
                      singleSignal += 1;

                      if(g_str_has_prefix(mount_uri,"burn:///"))
                      cdSignal += 1;
                      qDebug()<<"i want to see important you"<<mount_uri;
                      g_object_unref(volume);
                      g_object_unref(root);
                      g_free(mount_uri);
                  //    g_object_unref(volume);

                  }
              }
              g_list_free(listVolumes);
              hign = findGMountList()->size()*40 + findGDriveList()->size()*55;
              this->setFixedSize(280,hign);

              if(cdSignal)
              {
                  qDebug()<<"---------------mmmmm";
                  switch(g_list_length(g_drive_get_volumes(cacheDrive)))
                  {
                  qDebug()<<"wwj,nizhendeshiyige";
                      case 1:
                      //when the drive's volume number is 1
                      /*determine whether the drive is only one and whether if the drive is the fisrst one,
                       *if the answer is yes,we set the last parameter is 1.*/
                      if(findGDriveList()->size() == 1)
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL, 1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL,1);
                      }
                      else if(num == 1)
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL, 1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL,1);
                      }
                      else
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL,1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL, 2);
                      }
                      break;
                      case 2:
                      if(findGDriveList()->size() == 1)
                      {
                          newarea(2,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                  NULL,NULL, 1,1,NULL,NULL, "burn:///","burn:///",NULL,NULL,1);
                      }
                      else if(num == 1)
                      {
                          newarea(2,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                  NULL,NULL, 1,1,NULL,NULL, "burn:///","burn:///",NULL,NULL,1);
                      }
                      else
                      {
                          newarea(2,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                  NULL,NULL,1,1,NULL,NULL, "burn:///","burn:///",NULL,NULL, 2);
                      }
                      break;
                      default:
                      if(findGDriveList()->size() == 1)
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL, 1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL,1);
                      }
                      else if(num == 1)
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL, 1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL,1);
                      }
                      else
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL,1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL, 2);
                      }
                  }
              }
              else
              {
              qDebug()<<"+++++++++++++++";
              GList *volumeNumber = g_drive_get_volumes(cacheDrive);
              int DisNum = g_list_length(volumeNumber);
              driveMountNum = 0;
//              qDebug()<<"driveMountNum"<<driveMountNum;
//              qDebug()<<"zhen li ji shi dao li"<<mount_uri;

              if(singleSignal !=0 )
              {
              if (DisNum >0)
              {
                if (g_drive_can_eject(cacheDrive) || g_drive_can_stop(cacheDrive))
                {
                    /*
                     * to get the U disk partition's path,name,capacity and U disk's name,then we layout by the
                     * number of its volume
                     * */

                    //when the drive's volume number is 1
                    hign = findGMountList()->size()*40 + findGDriveList()->size()*55;
                    if(DisNum == 1)
                    {
                       num++; 
                       qDebug()<<"UDiskPath hasn't got over";
                       char *driveName = g_drive_get_name(cacheDrive);
                       GVolume *element = (GVolume *)g_list_nth_data(volumeNumber,0);
                       char *volumeName = g_volume_get_name(element);
                       GFile *fileRoot = g_mount_get_root(g_volume_get_mount(element));
                       UDiskPathDis1 = g_file_get_path(fileRoot);
                       GFile *file = g_file_new_for_path(UDiskPathDis1);
                       GFileInfo *info = g_file_query_filesystem_info(file,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                       totalDis1 = g_file_info_get_attribute_uint64(info,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                       //when the drive's volume number is 1
                       /*determine whether the drive is only one and whether if the drive is the fisrst one,
                        *if the answer is yes,we set the last parameter is 1.*/
                       if(num == 1)
                       {

                           newarea(DisNum,cacheDrive,driveName,
                                   volumeName,
                                   NULL,NULL,NULL, totalDis1,NULL,NULL,NULL, QString(UDiskPathDis1),NULL,NULL,NULL,1);
                       }
//                       else if(findGDriveList()->size() == 1)
//                       {
//                           newarea(DisNum,driveName,
//                                   volumeName,
//                                   NULL,NULL,NULL, totalDis1,NULL,NULL,NULL, QString(UDiskPathDis1),NULL,NULL,NULL,1);
//                       }
                       else
                       {
                           newarea(DisNum,cacheDrive,driveName,
                                   volumeName,
                                   NULL,NULL,NULL, totalDis1,NULL,NULL,NULL, QString(UDiskPathDis1),NULL,NULL,NULL,2);
                       }

                       g_free(driveName);
                       g_free(volumeName);
                       g_object_unref(element);
                       g_free(UDiskPathDis1);
                       g_object_unref(file);
                    }
                    //when the drive's volume number is 2
                    if(DisNum == 2)
                    {
                        num++;
                        UDiskPathDis1 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0))));
//                        QByteArray dateDis1 = UDiskPathDis1.toLocal8Bit();
//                        char *p_ChangeDis1 = dateDis1.data();
//                        GFile *fileDis1 = g_file_new_for_path(p_ChangeDis1);
                        GFile *fileDis1 = g_file_new_for_path(UDiskPathDis1);
                        GFileInfo *infoDis1 = g_file_query_filesystem_info(fileDis1,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE ,nullptr,nullptr);
                        totalDis1 = g_file_info_get_attribute_uint64(infoDis1,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                        qDebug()<<"UdiskPath2 hasn't got over";
                        UDiskPathDis2 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1))));
                        qDebug()<<"it has got over:";
//                        QByteArray dateDis2 = UDiskPathDis2.toLocal8Bit();
//                        char *p_ChangeDis2 = dateDis2.data();
                        GFile *fileDis2 = g_file_new_for_path(UDiskPathDis2);
                        GFileInfo *infoDis2 = g_file_query_filesystem_info(fileDis2,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                        totalDis2 = g_file_info_get_attribute_uint64(infoDis2,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        if(num == 1)
                        {
//                            newarea(DisNum,g_drive_get_name(cacheDrive),
//                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
//                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
//                                    NULL,NULL, totalDis1,totalDis2,NULL,NULL, UDiskPathDis1,UDiskPathDis2,NULL,NULL,1);
                            char *driveName = g_drive_get_name(cacheDrive);
                            GList *cacheDriveList = g_drive_get_volumes(cacheDrive);
                            char *volumeName1 = g_volume_get_name((GVolume *)g_list_nth_data(cacheDriveList,0));
                            char *volumeName2 = g_volume_get_name((GVolume *)g_list_nth_data(cacheDriveList,1));
                            newarea(DisNum,cacheDrive,driveName,
                                    volumeName1,
                                    volumeName2,
                                    NULL,NULL, totalDis1,totalDis2,NULL,NULL, QString(UDiskPathDis1),QString(UDiskPathDis2),NULL,NULL,1);
 //                           g_free(g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)));
 //                           g_list_free(g_drive_get_volumes(cacheDrive));
                            g_free(UDiskPathDis1);
                            g_free(UDiskPathDis2);
                            g_free(driveName);
                            g_free(volumeName1);
                            g_free(volumeName2);
                            g_list_free(cacheDriveList);
                        }

                        else if(findGDriveList()->size() == 1)
                        {
//                            newarea(DisNum,g_drive_get_name(cacheDrive),
//                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
//                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
//                                    NULL,NULL, totalDis1,totalDis2,NULL,NULL, UDiskPathDis1,UDiskPathDis2,NULL,NULL,1);
                            char *driveName = g_drive_get_name(cacheDrive);
                            qDebug()<<"list hasn't got over:";
                            GList *cacheDriveList = g_drive_get_volumes(cacheDrive);
                            qDebug()<<"Glist *cacheDriveList get over:";
                            char *volumeName1 = g_volume_get_name((GVolume *)g_list_nth_data(cacheDriveList,0));
                            char *volumeName2 = g_volume_get_name((GVolume *)g_list_nth_data(cacheDriveList,1));
                            newarea(DisNum,cacheDrive,driveName,
                                    volumeName1,
                                    volumeName2,
                                    NULL,NULL, totalDis1,totalDis2,NULL,NULL, QString(UDiskPathDis1),QString(UDiskPathDis2),NULL,NULL,1);
 //                           g_free(g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)));
 //                           g_list_free(g_drive_get_volumes(cacheDrive));
                            g_free(UDiskPathDis1);
                            g_free(UDiskPathDis2);
                            g_free(driveName);
                            g_free(volumeName1);
                            g_free(volumeName2);
                            g_list_free(cacheDriveList);
                        }

                        else
                        {
//                            newarea(DisNum,g_drive_get_name(cacheDrive),
//                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
//                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
//                                    NULL,NULL, totalDis1,totalDis2,NULL,NULL, UDiskPathDis1,UDiskPathDis2,NULL,NULL,2);
                            char *driveName = g_drive_get_name(cacheDrive);
                            GList *cacheDriveList = g_drive_get_volumes(cacheDrive);
                            qDebug()<<"cacheDriveList------"<<cacheDriveList;
                            char *volumeName1 = g_volume_get_name((GVolume *)g_list_nth_data(cacheDriveList,0));
                            char *volumeName2 = g_volume_get_name((GVolume *)g_list_nth_data(cacheDriveList,1));
                            newarea(DisNum,cacheDrive,driveName,
                                    volumeName1,
                                    volumeName2,
                                    NULL,NULL, totalDis1,NULL,NULL,NULL, QString(UDiskPathDis1),QString(UDiskPathDis2),NULL,NULL,2);
 //                           g_free(g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)));
 //                           g_list_free(g_drive_get_volumes(cacheDrive));
                            g_free(UDiskPathDis1);
                            g_free(UDiskPathDis2);
                            g_free(driveName);
                            g_free(volumeName1);
                            g_free(volumeName2);
                            g_list_free(cacheDriveList);
                        }
                        g_object_unref(fileDis2);

                    }
                    //when the drive's volume number is 3
                    if(DisNum == 3)
                    {
                        num++;
                        UDiskPathDis1 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0))));
//                        QByteArray dateDis1 = UDiskPathDis1.toLocal8Bit();
//                        char *p_ChangeDis1 = dateDis1.data();
                        GFile *fileDis1 = g_file_new_for_path(UDiskPathDis1);
                        GFileInfo *infoDis1 = g_file_query_filesystem_info(fileDis1,"*",nullptr,nullptr);
                        totalDis1 = g_file_info_get_attribute_uint64(infoDis1,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis2 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1))));
//                        QByteArray dateDis2 = UDiskPathDis2.toLocal8Bit();
//                        char *p_ChangeDis2 = dateDis2.data();
                        GFile *fileDis2 = g_file_new_for_path(UDiskPathDis2);
                        GFileInfo *infoDis2 = g_file_query_filesystem_info(fileDis2,"*",nullptr,nullptr);
                        totalDis2 = g_file_info_get_attribute_uint64(infoDis2,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis3 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2))));
//                        QByteArray dateDis3 = UDiskPathDis3.toLocal8Bit();
//                        char *p_ChangeDis3 = dateDis3.data();
                        GFile *fileDis3 = g_file_new_for_path(UDiskPathDis3);
                        GFileInfo *infoDis3 = g_file_query_filesystem_info(fileDis3,"*",nullptr,nullptr);
                        totalDis3 = g_file_info_get_attribute_uint64(infoDis3,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                        if(findGDriveList()->size() == 1)
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    NULL, totalDis1,totalDis2,totalDis3,NULL, QString(UDiskPathDis1),QString(UDiskPathDis2),QString(UDiskPathDis3),NULL,1);
                        }

                        else if(num == 1)
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    NULL, totalDis1,totalDis2,totalDis3,NULL, QString(UDiskPathDis1),QString(UDiskPathDis2),QString(UDiskPathDis3),NULL,1);
                        }

                        else
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    NULL, totalDis1,totalDis2,totalDis3,NULL, QString(UDiskPathDis1),QString(UDiskPathDis2),QString(UDiskPathDis3),NULL,2);
                        }

                    }
                    //when the drive's volume number is 4
                    if(DisNum == 4)
                    {
                        num++;
                        UDiskPathDis1 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0))));
//                        QByteArray dateDis1 = UDiskPathDis1.toLocal8Bit();
//                        char *p_ChangeDis1 = dateDis1.data();
                        GFile *fileDis1 = g_file_new_for_path(UDiskPathDis1);
                        GFileInfo *infoDis1 = g_file_query_filesystem_info(fileDis1,"*",nullptr,nullptr);
                        totalDis1 = g_file_info_get_attribute_uint64(infoDis1,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis2 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1))));
//                        QByteArray dateDis2 = UDiskPathDis2.toLocal8Bit();
//                        char *p_ChangeDis2 = dateDis2.data();
                        GFile *fileDis2 = g_file_new_for_path(UDiskPathDis2);
                        GFileInfo *infoDis2 = g_file_query_filesystem_info(fileDis2,"*",nullptr,nullptr);
                        totalDis2 = g_file_info_get_attribute_uint64(infoDis2,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis3 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2))));
//                        QByteArray dateDis3 = UDiskPathDis3.toLocal8Bit();
//                        char *p_ChangeDis3 = dateDis3.data();
                        GFile *fileDis3 = g_file_new_for_path(UDiskPathDis3);
                        GFileInfo *infoDis3 = g_file_query_filesystem_info(fileDis3,"*",nullptr,nullptr);
                        totalDis3 = g_file_info_get_attribute_uint64(infoDis3,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis4 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),3))));
//                        QByteArray dateDis4 = UDiskPathDis4.toLocal8Bit();
//                        char *p_ChangeDis4 = dateDis4.data();
                        GFile *fileDis4 = g_file_new_for_path(UDiskPathDis4);
                        GFileInfo *infoDis4 = g_file_query_filesystem_info(fileDis4,"*",nullptr,nullptr);
                        totalDis4 = g_file_info_get_attribute_uint64(infoDis4,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        if(findGDriveList()->size() == 1)
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),3)),
                                    totalDis1,totalDis2,totalDis3,totalDis4, QString(UDiskPathDis1),QString(UDiskPathDis2),QString(UDiskPathDis3),QString(UDiskPathDis4),1);
                        }

                        else if(num == 1)
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),3)),
                                    totalDis1,totalDis2,totalDis3,totalDis4, UDiskPathDis1,UDiskPathDis2,UDiskPathDis3,UDiskPathDis4,1);
                        }

                        else
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),3)),
                                    totalDis1,totalDis2,totalDis3,totalDis4, UDiskPathDis1,UDiskPathDis2,UDiskPathDis3,UDiskPathDis4,2);
                        }

                    }
                }
              }
              }
              }

//              connect(this, &MainWindow::unloadMount,this,[=]()
//              {
//                  if(open_widget)
//                  delete open_widget;
//              });


//              {
//                  connect(open_widget, &QClickWidget::clickedConvert,this,[=]()
//                  {
//                      qDebug()<<g_drive_get_name(cacheDrive)<<"1----------------2";
//                      g_drive_eject_with_operation(cacheDrive,
//                                   G_MOUNT_UNMOUNT_NONE,
//                                   NULL,
//                                   NULL,
//                                   GAsyncReadyCallback(frobnitz_result_func),
//                                   this);
//                      this->hide();

////                        if(g_drive_can_eject)
////                        ForEject->show();
//                      QLayoutItem* item;
//                      while ((item = this->vboxlayout->takeAt(0)) != NULL)
//                      {
//                          delete item->widget();
//                          delete item;
//                      }
//                      qDebug()<<"findGDriveList.num"<<findGDriveList()->size();
//                      qDebug()<<"findGMountList.num"<<findGMountList()->size();
//                      if(findGDriveList()->size() == 0 || findGMountList()->size() ==0)
//                      {
//                          m_systray->hide();
//                      }
//                  });
//              }

                if(findGDriveList()->size() != 0)
                {
                    this->showNormal();
                    moveBottomRight();
                }



      }
    }
    else
    {
      this->hide();
    }
    break;
    default:
        break;
    }
    ui->centralWidget->show();

}

void MainWindow::hideEvent(QHideEvent event)
{
    qDebug()<<"delete open widget";
    delete open_widget;
}


/*
 * newarea use all the information of the U disk to paint the main interface and add line
*/
void MainWindow::newarea(int No,
                         GDrive *Drive,
                         QString Drivename,
                         QString nameDis1,
                         QString nameDis2,
                         QString nameDis3,
                         QString nameDis4,
                         qlonglong capacityDis1,
                         qlonglong capacityDis2,
                         qlonglong capacityDis3,
                         qlonglong capacityDis4,
                         QString pathDis1,
                         QString pathDis2,
                         QString pathDis3,
                         QString pathDis4,
                         int linestatus)
{
    if(open_widget)
    {
//        qDebug()<<"open_widget reel"<<open_widget;
//        delete open_widget;
    }
    open_widget = new QClickWidget(this,No,Drive,Drivename,nameDis1,nameDis2,nameDis3,nameDis4,
                                   capacityDis1,capacityDis2,capacityDis3,capacityDis4,
                                   pathDis1,pathDis2,pathDis3,pathDis4);
    connect(open_widget,&QClickWidget::clickedConvert,this,[=]()
    {
        this->hide();
    });


    QWidget *line = new QWidget;
    line->setFixedHeight(1);
    line->setObjectName("lineWidget");

    //when the drive is only or the drive is the first one,we make linestatus become  1
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    if (linestatus == 2)
    {
        this->vboxlayout->addWidget(line);
    }

    this->vboxlayout->addWidget(open_widget);
    vboxlayout->setContentsMargins(2,8,2,8);

    if (linestatus == 0)
    {
        this->vboxlayout->addWidget(line);
    }

    open_widget->setStyleSheet(
                //正常状态样式
                "QClickWidget{"
                "height:79px;"
                "}"
                "QClickWidget:hover{"
                "background-color:rgba(255,255,255,30);"
                "}"
                );

}

void MainWindow::moveBottomRight()
{
////////////////////////////////////////get the loacation of the mouse
    /*QPoint globalCursorPos = QCursor::pos();

    QRect availableGeometry = qApp->primaryScreen()->availableGeometry();
    QRect screenGeometry = qApp->primaryScreen()->geometry();

    QDesktopWidget* pDesktopWidget = QApplication::desktop();

    QRect screenRect = pDesktopWidget->screenGeometry();//area of the screen

    if (screenRect.height() != availableGeometry.height())
    {
        this->move(globalCursorPos.x()-125, availableGeometry.height() - this->height());
    }
    else
    {
        this->move(globalCursorPos.x()-125, availableGeometry.height() - this->height() - 40);
    }*/
    //////////////////////////////////////origin code base on the location of the mouse
//    int screenNum = QGuiApplication::screens().count();
//    qDebug()<<"ScreenNum:"<<"-------------"<<screenNum;
//    int panelHeight = getPanelHeight("PanelHeight");
//    int position =0;
//    position = getPanelPosition("PanelPosion");
//    int screen = 0;
//    QRect rect;
//    int localX ,availableWidth,totalWidth;
//    int localY,availableHeight,totalHeight;

//    qDebug() << "任务栏位置"<< position;
//    if (screenNum > 1)
//    {
//        if (position == rightPosition)                                  //on the right
//        {
//            screen = screenNum - 1;

//            //Available screen width and height
//            availableWidth =QGuiApplication::screens().at(screen)->geometry().x() +  QGuiApplication::screens().at(screen)->size().width()-panelHeight;
//            qDebug()<<QGuiApplication::screens().at(screen)->geometry().x()<<"QGuiApplication::screens().at(screen)->geometry().x()";
//            qDebug()<<QGuiApplication::screens().at(screen)->size().width()<<"QGuiApplication::screens().at(screen)->size().width()";
//            availableHeight = QGuiApplication::screens().at(screen)->availableGeometry().height();

//            //total width
//            totalWidth =  QGuiApplication::screens().at(0)->size().width() + QGuiApplication::screens().at(screen)->size().width();
//            totalHeight = QGuiApplication::screens().at(screen)->size().height();
//            this->move(totalWidth,totalHeight);
//        }
//        else if(position  ==downPosition || upPosition ==1)                  //above or bellow
//        {
//            availableHeight = QGuiApplication::screens().at(0)->size().height() - panelHeight;
//            availableWidth = QGuiApplication::screens().at(0)->size().width();
//            totalHeight = QGuiApplication::screens().at(0)->size().height();
//            totalWidth = QGuiApplication::screens().at(0)->size().width();
//        }
//        else
//        {
//            availableHeight = QGuiApplication::screens().at(0)->availableGeometry().height();
//            availableWidth = QGuiApplication::screens().at(0)->availableGeometry().width();
//            totalHeight = QGuiApplication::screens().at(0)->size().height();
//            totalWidth = QGuiApplication::screens().at(0)->size().width();
//        }
//    }

//    else
//    {
//        availableHeight = QGuiApplication::screens().at(0)->availableGeometry().height();
//        availableWidth = QGuiApplication::screens().at(0)->availableGeometry().width();
//        totalHeight = QGuiApplication::screens().at(0)->size().height();
//        totalWidth = QGuiApplication::screens().at(0)->size().width();

//        //show the location of the systemtray
//        rect = m_systray->geometry();
//        localX = rect.x() - (this->width()/2 - rect.size().width()/2) ;
//        localY = availableHeight - this->height();

//        //modify location
//        if (position == downPosition)
//        { //下
//            if (availableWidth - rect.x() - rect.width()/2 < this->width() / 2)
//                this->setGeometry(availableWidth-this->width(),availableHeight-this->height()-DistanceToPanel,this->width(),this->height());
//            else
//                this->setGeometry(localX,availableHeight-this->height()-DistanceToPanel,this->width(),this->height());
//        }
//        else if (position == upPosition)
//        { //上
//            if (availableWidth - rect.x() - rect.width()/2 < this->width() / 2)
//                this->setGeometry(availableWidth-this->width(),totalHeight-availableHeight+DistanceToPanel,this->width(),this->height());
//            else
//                this->setGeometry(localX,totalHeight-availableHeight+DistanceToPanel,this->width(),this->height());
//        }
//        else if (position == leftPosition)
//        {
//            if (availableHeight - rect.y() - rect.height()/2 > this->height() /2)
//                this->setGeometry(panelHeight + DistanceToPanel,rect.y() + (rect.width() /2) -(this->height()/2) ,this->width(),this->height());
//            else
//                this->setGeometry(panelHeight+DistanceToPanel,localY,this->width(),this->height());//左
//        }
//        else if (position == rightPosition)
//        {
//            localX = availableWidth - this->width();
//            if (availableHeight - rect.y() - rect.height()/2 > this->height() /2)
//                this->setGeometry(availableWidth - this->width() -DistanceToPanel,rect.y() + (rect.width() /2) -(this->height()/2),this->width(),this->height());
//            else
//                this->setGeometry(localX-DistanceToPanel,localY,this->width(),this->height());
//        }
//    }
    QRect availableGeometry = qApp->primaryScreen()->availableGeometry();
    QRect screenGeometry = qApp->primaryScreen()->geometry();

    QDesktopWidget* desktopWidget = QApplication::desktop();
    QRect deskMainRect = desktopWidget->availableGeometry(0);//获取可用桌面大小
    QRect screenMainRect = desktopWidget->screenGeometry(0);//获取设备屏幕大小
    QRect deskDupRect = desktopWidget->availableGeometry(1);//获取可用桌面大小
    QRect screenDupRect = desktopWidget->screenGeometry(1);//获取设备屏幕大小

    int PanelPosition = getPanelPosition("position");     //n
    int PanelHeight = getPanelHeight("height");                   //m
//    int d = 2; //窗口边沿到任务栏距离

    if (screenGeometry.width() == availableGeometry.width() && screenGeometry.height() == availableGeometry.height()) {
        if (PanelPosition == 0) {
            //任务栏在下侧
            this->move(availableGeometry.x() + availableGeometry.width() - this->width(), screenMainRect.y() + availableGeometry.height() - this->height() - PanelHeight - DistanceToPanel);
        } else if(PanelPosition == 1) {
            //任务栏在上侧
            this->move(availableGeometry.x() + availableGeometry.width() - this->width(), screenMainRect.y() + screenGeometry.height() - availableGeometry.height() + PanelHeight + DistanceToPanel);
        } else if (PanelPosition == 2) {
            //任务栏在左侧
            if (screenGeometry.x() == 0) {//主屏在左侧
                this->move(PanelHeight + DistanceToPanel, screenMainRect.y() + screenMainRect.height() - this->height());
            } else {//主屏在右侧
                this->move(screenMainRect.x() + PanelHeight + DistanceToPanel, screenMainRect.y() + screenMainRect.height() - this->height());
            }
        } else if (PanelPosition == 3) {
            //任务栏在右侧
            if (screenGeometry.x() == 0) {//主屏在左侧
                this->move(screenMainRect.width() - this->width() - PanelHeight - DistanceToPanel, screenMainRect.y() + screenMainRect.height() - this->height());
            } else {//主屏在右侧
                this->move(screenMainRect.x() + screenMainRect.width() - this->width() - PanelHeight - DistanceToPanel, screenMainRect.y() + screenMainRect.height() - this->height());
            }
        }
    } else if(screenGeometry.width() == availableGeometry.width() ) {
        if (m_systray->geometry().y() > availableGeometry.height()/2) {
            //任务栏在下侧
            this->move(availableGeometry.x() + availableGeometry.width() - this->width(), screenMainRect.y() + availableGeometry.height() - this->height() - DistanceToPanel);
        } else {
            //任务栏在上侧
            this->move(availableGeometry.x() + availableGeometry.width() - this->width(), screenMainRect.y() + screenGeometry.height() - availableGeometry.height() + DistanceToPanel);
        }
    } else if (screenGeometry.height() == availableGeometry.height()) {
        if (m_systray->geometry().x() > availableGeometry.width()/2) {
            //任务栏在右侧
            this->move(availableGeometry.x() + availableGeometry.width() - this->width() - DistanceToPanel, screenMainRect.y() + screenGeometry.height() - this->height());
        } else {
            //任务栏在左侧
            this->move(screenGeometry.width() - availableGeometry.width() + DistanceToPanel, screenMainRect.y() + screenGeometry.height() - this->height());
        }
    }
}

/*
    use dbus to get the location of the panel
*/
int MainWindow::getPanelPosition(QString str)
{
    QDBusInterface interface( "com.ukui.panel.desktop",
                              "/",
                              "com.ukui.panel.desktop",
                              QDBusConnection::sessionBus() );
    QDBusReply<int> reply = interface.call("GetPanelPosition", str);

    return reply;
}

/*
    use the dbus to get the height of the panel
*/
int MainWindow::getPanelHeight(QString str)
{
    QDBusInterface interface( "com.ukui.panel.desktop",
                              "/",
                              "com.ukui.panel.desktop",
                              QDBusConnection::sessionBus() );
    QDBusReply<int> reply = interface.call("GetPanelSize", str);
    return reply;
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
}

void MainWindow::moveBottomDirect(GDrive *drive)
{
//    ejectInterface *ForEject = new ejectInterface(nullptr,g_drive_get_name(drive));
//    int screenNum = QGuiApplication::screens().count();
//    int panelHeight = getPanelHeight("PanelHeight");
//    int position =0;
//    position = getPanelPosition("PanelPosion");
//    int screen = 0;
//    QRect rect;
//    int localX ,availableWidth,totalWidth;
//    int localY,availableHeight,totalHeight;

//    qDebug() << "任务栏位置"<< position;
//    if (screenNum > 1)
//    {
//        if (position == rightPosition)                                  //on the right
//        {
//            screen = screenNum - 1;

//            //Available screen width and height
//            availableWidth =QGuiApplication::screens().at(screen)->geometry().x() +  QGuiApplication::screens().at(screen)->size().width()-panelHeight;
//            availableHeight = QGuiApplication::screens().at(screen)->availableGeometry().height();

//            //total width
//            totalWidth =  QGuiApplication::screens().at(0)->size().width() + QGuiApplication::screens().at(screen)->size().width();
//            totalHeight = QGuiApplication::screens().at(screen)->size().height();
//        }
//        else if(position  ==downPosition || position ==upPosition)                  //above or bellow
//        {
//            availableHeight = QGuiApplication::screens().at(0)->size().height() - panelHeight;
//            availableWidth = QGuiApplication::screens().at(0)->size().width();
//            totalHeight = QGuiApplication::screens().at(0)->size().height();
//            totalWidth = QGuiApplication::screens().at(0)->size().width();
//        }
//        else
//        {
//            availableHeight = QGuiApplication::screens().at(0)->availableGeometry().height();
//            availableWidth = QGuiApplication::screens().at(0)->availableGeometry().width();
//            totalHeight = QGuiApplication::screens().at(0)->size().height();
//            totalWidth = QGuiApplication::screens().at(0)->size().width();
//        }
//    }

//    else
//    {
//        availableHeight = QGuiApplication::screens().at(0)->availableGeometry().height();
//        availableWidth = QGuiApplication::screens().at(0)->availableGeometry().width();
//        totalHeight = QGuiApplication::screens().at(0)->size().height();
//        totalWidth = QGuiApplication::screens().at(0)->size().width();
//    }
//    //show the location of the systemtray
//    rect = m_systray->geometry();
//    localX = rect.x() - (ForEject->width()/2 - rect.size().width()/2) ;
//    localY = availableHeight - ForEject->height();
//    //modify location
//    if (position == downPosition)
//    { //下
//        if (availableWidth - rect.x() - rect.width()/2 < ForEject->width() / 2)
//            ForEject->setGeometry(availableWidth-ForEject->width(),availableHeight-ForEject->height()-DistanceToPanel,ForEject->width(),ForEject->height());
//        else
//            ForEject->setGeometry(localX-16,availableHeight-ForEject->height()-DistanceToPanel,ForEject->width(),ForEject->height());
//    }
//    else if (position == upPosition)
//    { //上
//        if (availableWidth - rect.x() - rect.width()/2 < ForEject->width() / 2)
//            ForEject->setGeometry(availableWidth-ForEject->width(),totalHeight-availableHeight+DistanceToPanel,ForEject->width(),ForEject->height());
//        else
//            ForEject->setGeometry(localX-16,totalHeight-availableHeight+DistanceToPanel,ForEject->width(),ForEject->height());
//    }
//    else if (position == leftPosition)
//    {
//        if (availableHeight - rect.y() - rect.height()/2 > ForEject->height() /2)
//            ForEject->setGeometry(panelHeight + DistanceToPanel,rect.y() + (rect.width() /2) -(ForEject->height()/2) ,ForEject->width(),ForEject->height());
//        else
//            ForEject->setGeometry(panelHeight+DistanceToPanel,localY,ForEject->width(),ForEject->height());//左
//    }
//    else if (position == rightPosition)
//    {
//        localX = availableWidth - ForEject->width();
//        if (availableHeight - rect.y() - rect.height()/2 > ForEject->height() /2)
//        {
//            ForEject->setGeometry(availableWidth - ForEject->width() -DistanceToPanel,rect.y() + (rect.height() /2) -(ForEject->height()/2),ForEject->width(),ForEject->height());
//        }
//        else
//            ForEject->setGeometry(localX-DistanceToPanel,localY,ForEject->width(),ForEject->height());
//    }
//    ForEject->show();
}

void MainWindow::MainWindowShow()
{
    ui->centralWidget->setObjectName("centralWidget");
    ui->centralWidget->setStyleSheet(
                "#centralWidget{"
                "width:280px;"
                "height:192px;"
                "border:1px solid rgba(255, 255, 255, 0.05);"
                "opacity:0.75;"

                "border-radius:6px;"
                "box-shadow:0px 2px 6px 0px rgba(0, 0, 0, 0.2);"
//                "margin:0px;"
//                "border-width:0px;"
//                "padding:0px;"
                "}"
                );
//    m_transparency = this->getTransparentData();

//    QString strTrans;
//    strTrans =  QString::number(m_transparency, 10, 2);
//    qDebug()<<"strTrans---"<<strTrans;
    QString convertStyle = "#centralWidget{background:rgba(19,19,20,0.70);}";
    this->setStyleSheet(convertStyle);

    int num = 0;
    if  ( this->vboxlayout != NULL )
    {
        QLayoutItem* item;
        while ((item = this->vboxlayout->takeAt(0)) != NULL)
        {
            delete item->widget();
            delete item;
        }
    }
//      if (this->isHidden())
//      {

          //MainWindow::hign = MainWindow::oneVolumeDriveNum*98+MainWindow::twoVolumeDriveNum*110+MainWindow::threeVolumeDriveNum*130+MainWindow::fourVolumeDriveNum*160;
          for(auto cacheDrive : *findGDriveList())
          {

              qDebug()<<"findGDriveList().num"<<findGDriveList()->size();
              qDebug()<<"findGMountList().num"<<findGMountList()->size();

              int singleSignal = 0;
              int cdSignal = 0;
              listVolumes = g_drive_get_volumes(cacheDrive);
              for(vList = listVolumes; vList != NULL; vList = vList->next)
              {
                  volume = (GVolume *)vList->data;
                  if(g_volume_get_mount(volume) != NULL)
                  {
                      root = g_mount_get_default_location(g_volume_get_mount(volume));
                      mount_uri = g_file_get_uri(root);
                      if(g_str_has_prefix(mount_uri,"file:///"))
                      singleSignal += 1;

                      if(g_str_has_prefix(mount_uri,"burn:///"))
                      cdSignal += 1;
                      qDebug()<<"i want to see important you"<<mount_uri;
                      g_object_unref(volume);
                      g_object_unref(root);
                      g_free(mount_uri);
                  }
              }
              g_list_free(listVolumes);

              hign = findGVolumeList()->size()*40 + findGDriveList()->size()*55;
              this->setFixedSize(280,hign);

              if(strcmp(mount_uri,"burn:///")==0)
              {
                  qDebug()<<"---------------mmmmm";
                  switch(g_list_length(g_drive_get_volumes(cacheDrive)))
                  {
                  qDebug()<<"wwj,nizhendeshiyige";
                      case 1:
                      //when the drive's volume number is 1
                      /*determine whether the drive is only one and whether if the drive is the fisrst one,
                       *if the answer is yes,we set the last parameter is 1.*/
                      if(findGDriveList()->size() == 1)
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL, 1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL,1);
                      }
                      else if(num == 1)
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL, 1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL,1);
                      }
                      else
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL,1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL, 2);
                      }
                      break;
                      case 2:
                      qDebug()<<"wwwwwjjjjj,dasdaddsb";
                      if(findGDriveList()->size() == 1)
                      {
                          newarea(2,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                  NULL,NULL, 1,1,NULL,NULL, "burn:///","burn:///",NULL,NULL,1);
                      }
                      else if(num == 1)
                      {
                          newarea(2,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                  NULL,NULL, 1,1,NULL,NULL, "burn:///","burn:///",NULL,NULL,1);
                      }
                      else
                      {
                          newarea(2,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                  NULL,NULL,1,1,NULL,NULL, "burn:///","burn:///",NULL,NULL, 2);
                      }
                      break;
                      default:
                      if(findGDriveList()->size() == 1)
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL, 1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL,1);
                      }
                      else if(num == 1)
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL, 1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL,1);
                      }
                      else
                      {
                          newarea(1,cacheDrive,g_drive_get_name(cacheDrive),
                                  g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                  NULL,NULL,NULL,1,NULL,NULL,NULL, "burn:///",NULL,NULL,NULL, 2);
                      }


                  }
                  g_object_unref(root);
                  g_free(mount_uri);
                  g_object_unref(volume);
                  g_list_free(listVolumes);
              }
              else
              {
              if(singleSignal != 0)
              {
              g_drive_get_volumes(cacheDrive);
              int DisNum = g_list_length(g_drive_get_volumes(cacheDrive));
              if (DisNum >0 )
              {
                if (g_drive_can_eject(cacheDrive) || g_drive_can_stop(cacheDrive))
                {

                    if(DisNum == 1)
                    {
                       num++;
//                       GMount *pathMount1 = g_volume_get_mount((GVolume *)g_list_nth_data(volumeNumber,0));
//                       GFile *pathFile1 = g_mount_get_root(pathMount1);
//                       UDiskPathDis1 = g_file_get_path(pathFile1);
                       UDiskPathDis1 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0))));
//                       QByteArray date = UDiskPathDis1.toLocal8Bit();
//                       char *p_Change = date.data();
                       GFile *file = g_file_new_for_path(UDiskPathDis1);
                       GFileInfo *info = g_file_query_filesystem_info(file,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                       totalDis1 = g_file_info_get_attribute_uint64(info,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                       //when the drive's volume number is 1
                       /*determine whether the drive is only one and whether if the drive is the fisrst one,
                        *if the answer is yes,we set the last parameter is 1.*/
                       if(findGDriveList()->size() == 1)
                       {
                           char *driveName = g_drive_get_name(cacheDrive);
                           GList *cacheDriveList = g_drive_get_volumes(cacheDrive);
                           char *volumeName = g_volume_get_name((GVolume *)g_list_nth_data(cacheDriveList,0));
                           newarea(DisNum,cacheDrive,driveName,
                                   volumeName,
                                   NULL,NULL,NULL, totalDis1,NULL,NULL,NULL, QString(UDiskPathDis1),NULL,NULL,NULL,1);
//                           g_free(g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)));
//                           g_list_free(g_drive_get_volumes(cacheDrive));
                           g_free(UDiskPathDis1);
                           g_free(driveName);
                           g_free(volumeName);
                           g_list_free(cacheDriveList);
                       }
                       else if(num == 1)
                       {
//                           newarea(DisNum,g_drive_get_name(cacheDrive),
//                                   g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
//                                   NULL,NULL,NULL, totalDis1,NULL,NULL,NULL, UDiskPathDis1,NULL,NULL,NULL,1);
//                           g_free(g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)));
//                           g_list_free(g_drive_get_volumes(cacheDrive));
                           char *driveName = g_drive_get_name(cacheDrive);
                           GList *cacheDriveList = g_drive_get_volumes(cacheDrive);
                           char *volumeName = g_volume_get_name((GVolume *)g_list_nth_data(cacheDriveList,0));
                           newarea(DisNum,cacheDrive,driveName,
                                   volumeName,
                                   NULL,NULL,NULL, totalDis1,NULL,NULL,NULL, QString(UDiskPathDis1),NULL,NULL,NULL,1);
//                           g_free(g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)));
//                           g_list_free(g_drive_get_volumes(cacheDrive));
                           g_free(UDiskPathDis1);
                           g_free(driveName);
                           g_free(volumeName);
                           g_list_free(cacheDriveList);
                       }
                       else
                       {
//                           newarea(DisNum,g_drive_get_name(cacheDrive),
//                                   g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
//                                   NULL,NULL,NULL,totalDis1,NULL,NULL,NULL, UDiskPathDis1,NULL,NULL,NULL, 2);
//                           g_free(g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)));
//                           g_list_free(g_drive_get_volumes(cacheDrive));
                           char *driveName = g_drive_get_name(cacheDrive);
                           GList *cacheDriveList = g_drive_get_volumes(cacheDrive);
                           char *volumeName = g_volume_get_name((GVolume *)g_list_nth_data(cacheDriveList,0));
                           newarea(DisNum,cacheDrive,driveName,
                                   volumeName,
                                   NULL,NULL,NULL, totalDis1,NULL,NULL,NULL, QString(UDiskPathDis1),NULL,NULL,NULL,2);
//                           g_free(g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)));
//                           g_list_free(g_drive_get_volumes(cacheDrive));
                           g_free(UDiskPathDis1);
                           g_free(driveName);
                           g_free(volumeName);
                           g_list_free(cacheDriveList);
                       }

                       g_object_unref(file);
                    }
                    //when the drive's volume number is 2
                    if(DisNum == 2)
                    {
                        num++;
                        UDiskPathDis1 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0))));
//                        QByteArray dateDis1 = UDiskPathDis1.toLocal8Bit();
//                        char *p_ChangeDis1 = dateDis1.data();
//                        GFile *fileDis1 = g_file_new_for_path(p_ChangeDis1);
                        GFile *fileDis1 = g_file_new_for_path(UDiskPathDis1);
                        GFileInfo *infoDis1 = g_file_query_filesystem_info(fileDis1,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE ,nullptr,nullptr);
                        totalDis1 = g_file_info_get_attribute_uint64(infoDis1,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis2 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1))));
//                        QByteArray dateDis2 = UDiskPathDis2.toLocal8Bit();
//                        char *p_ChangeDis2 = dateDis2.data();
                        GFile *fileDis2 = g_file_new_for_path(UDiskPathDis2);
                        GFileInfo *infoDis2 = g_file_query_filesystem_info(fileDis2,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE,nullptr,nullptr);
                        totalDis2 = g_file_info_get_attribute_uint64(infoDis2,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        if(findGDriveList()->size() == 1)
                        {
//                            newarea(DisNum,g_drive_get_name(cacheDrive),
//                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
//                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
//                                    NULL,NULL, totalDis1,totalDis2,NULL,NULL, UDiskPathDis1,UDiskPathDis2,NULL,NULL,1);
                            char *driveName = g_drive_get_name(cacheDrive);
                            GList *cacheDriveList = g_drive_get_volumes(cacheDrive);
                            char *volumeName1 = g_volume_get_name((GVolume *)g_list_nth_data(cacheDriveList,0));
                            char *volumeName2 = g_volume_get_name((GVolume *)g_list_nth_data(cacheDriveList,1));
                            newarea(DisNum,cacheDrive,driveName,
                                    volumeName1,
                                    volumeName2,
                                    NULL,NULL, totalDis1,totalDis2,NULL,NULL, QString(UDiskPathDis1),QString(UDiskPathDis2),NULL,NULL,1);
 //                           g_free(g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)));
 //                           g_list_free(g_drive_get_volumes(cacheDrive));
                            g_free(UDiskPathDis1);
                            g_free(UDiskPathDis2);
                            g_free(driveName);
                            g_free(volumeName1);
                            g_free(volumeName2);
                            g_list_free(cacheDriveList);
                        }

                        else if(num == 1)
                        {
//                            newarea(DisNum,g_drive_get_name(cacheDrive),
//                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
//                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
//                                    NULL,NULL, totalDis1,totalDis2,NULL,NULL, UDiskPathDis1,UDiskPathDis2,NULL,NULL,1);
                            char *driveName = g_drive_get_name(cacheDrive);
                            GList *cacheDriveList = g_drive_get_volumes(cacheDrive);
                            char *volumeName1 = g_volume_get_name((GVolume *)g_list_nth_data(cacheDriveList,0));
                            char *volumeName2 = g_volume_get_name((GVolume *)g_list_nth_data(cacheDriveList,1));
                            newarea(DisNum,cacheDrive,driveName,
                                    volumeName1,
                                    volumeName2,
                                    NULL,NULL, totalDis1,totalDis2,NULL,NULL, QString(UDiskPathDis1),QString(UDiskPathDis2),NULL,NULL,1);
 //                           g_free(g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)));
 //                           g_list_free(g_drive_get_volumes(cacheDrive));
                            g_free(UDiskPathDis1);
                            g_free(UDiskPathDis2);
                            g_free(driveName);
                            g_free(volumeName1);
                            g_free(volumeName2);
                            g_list_free(cacheDriveList);
                        }

                        else
                        {
//                            newarea(DisNum,g_drive_get_name(cacheDrive),
//                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
//                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
//                                    NULL,NULL, totalDis1,totalDis2,NULL,NULL, UDiskPathDis1,UDiskPathDis2,NULL,NULL,2);
                            char *driveName = g_drive_get_name(cacheDrive);
                            GList *cacheDriveList = g_drive_get_volumes(cacheDrive);
                            char *volumeName1 = g_volume_get_name((GVolume *)g_list_nth_data(cacheDriveList,0));
                            char *volumeName2 = g_volume_get_name((GVolume *)g_list_nth_data(cacheDriveList,1));
                            newarea(DisNum,cacheDrive,driveName,
                                    volumeName1,
                                    volumeName2,
                                    NULL,NULL, totalDis1,NULL,NULL,NULL, QString(UDiskPathDis1),QString(UDiskPathDis2),NULL,NULL,2);
 //                           g_free(g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)));
 //                           g_list_free(g_drive_get_volumes(cacheDrive));
                            g_free(UDiskPathDis1);
                            g_free(UDiskPathDis2);
                            g_free(driveName);
                            g_free(volumeName1);
                            g_free(volumeName2);
                            g_list_free(cacheDriveList);
                        }
                        g_object_unref(fileDis2);

                    }
                    //when the drive's volume number is 3
                    if(DisNum == 3)
                    {
                        num++;
                        UDiskPathDis1 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0))));
//                        QByteArray dateDis1 = UDiskPathDis1.toLocal8Bit();
//                        char *p_ChangeDis1 = dateDis1.data();
                        GFile *fileDis1 = g_file_new_for_path(UDiskPathDis1);
                        GFileInfo *infoDis1 = g_file_query_filesystem_info(fileDis1,"*",nullptr,nullptr);
                        totalDis1 = g_file_info_get_attribute_uint64(infoDis1,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis2 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1))));
//                        QByteArray dateDis2 = UDiskPathDis2.toLocal8Bit();
//                        char *p_ChangeDis2 = dateDis2.data();
                        GFile *fileDis2 = g_file_new_for_path(UDiskPathDis2);
                        GFileInfo *infoDis2 = g_file_query_filesystem_info(fileDis2,"*",nullptr,nullptr);
                        totalDis2 = g_file_info_get_attribute_uint64(infoDis2,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis3 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2))));
//                        QByteArray dateDis3 = UDiskPathDis3.toLocal8Bit();
//                        char *p_ChangeDis3 = dateDis3.data();
                        GFile *fileDis3 = g_file_new_for_path(UDiskPathDis3);
                        GFileInfo *infoDis3 = g_file_query_filesystem_info(fileDis3,"*",nullptr,nullptr);
                        totalDis3 = g_file_info_get_attribute_uint64(infoDis3,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
                        if(findGDriveList()->size() == 1)
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    NULL, totalDis1,totalDis2,totalDis3,NULL, QString(UDiskPathDis1),QString(UDiskPathDis2),QString(UDiskPathDis3),NULL,1);
                        }

                        else if(num == 1)
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    NULL, totalDis1,totalDis2,totalDis3,NULL, QString(UDiskPathDis1),QString(UDiskPathDis2),QString(UDiskPathDis3),NULL,1);
                        }

                        else
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    NULL, totalDis1,totalDis2,totalDis3,NULL, QString(UDiskPathDis1),QString(UDiskPathDis2),QString(UDiskPathDis3),NULL,2);
                        }

                    }
                    //when the drive's volume number is 4
                    if(DisNum == 4)
                    {
                        num++;
                        UDiskPathDis1 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0))));
//                        QByteArray dateDis1 = UDiskPathDis1.toLocal8Bit();
//                        char *p_ChangeDis1 = dateDis1.data();
                        GFile *fileDis1 = g_file_new_for_path(UDiskPathDis1);
                        GFileInfo *infoDis1 = g_file_query_filesystem_info(fileDis1,"*",nullptr,nullptr);
                        totalDis1 = g_file_info_get_attribute_uint64(infoDis1,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis2 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1))));
//                        QByteArray dateDis2 = UDiskPathDis2.toLocal8Bit();
//                        char *p_ChangeDis2 = dateDis2.data();
                        GFile *fileDis2 = g_file_new_for_path(UDiskPathDis2);
                        GFileInfo *infoDis2 = g_file_query_filesystem_info(fileDis2,"*",nullptr,nullptr);
                        totalDis2 = g_file_info_get_attribute_uint64(infoDis2,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis3 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2))));
//                        QByteArray dateDis3 = UDiskPathDis3.toLocal8Bit();
//                        char *p_ChangeDis3 = dateDis3.data();
                        GFile *fileDis3 = g_file_new_for_path(UDiskPathDis3);
                        GFileInfo *infoDis3 = g_file_query_filesystem_info(fileDis3,"*",nullptr,nullptr);
                        totalDis3 = g_file_info_get_attribute_uint64(infoDis3,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        UDiskPathDis4 = g_file_get_path(g_mount_get_root(g_volume_get_mount((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),3))));
//                        QByteArray dateDis4 = UDiskPathDis4.toLocal8Bit();
//                        char *p_ChangeDis4 = dateDis4.data();
                        GFile *fileDis4 = g_file_new_for_path(UDiskPathDis4);
                        GFileInfo *infoDis4 = g_file_query_filesystem_info(fileDis4,"*",nullptr,nullptr);
                        totalDis4 = g_file_info_get_attribute_uint64(infoDis4,G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

                        if(findGDriveList()->size() == 1)
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),3)),
                                    totalDis1,totalDis2,totalDis3,totalDis4, QString(UDiskPathDis1),QString(UDiskPathDis2),QString(UDiskPathDis3),QString(UDiskPathDis4),1);
                        }

                        else if(num == 1)
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),3)),
                                    totalDis1,totalDis2,totalDis3,totalDis4, UDiskPathDis1,UDiskPathDis2,UDiskPathDis3,UDiskPathDis4,1);
                        }

                        else
                        {
                            newarea(DisNum,cacheDrive,g_drive_get_name(cacheDrive),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),0)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),1)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),2)),
                                    g_volume_get_name((GVolume *)g_list_nth_data(g_drive_get_volumes(cacheDrive),3)),
                                    totalDis1,totalDis2,totalDis3,totalDis4, UDiskPathDis1,UDiskPathDis2,UDiskPathDis3,UDiskPathDis4,2);
                        }

                    }

              }
              }
              }
              }
//                    connect(open_widget, &QClickWidget::clickedConvert,this,[=]()
//                    {

//                        g_drive_eject_with_operation(cacheDrive,
//                                     G_MOUNT_UNMOUNT_NONE,
//                                     NULL,
//                                     NULL,
//                                     GAsyncReadyCallback(frobnitz_result_func),
//                                     this);

////                        findGDriveList()->removeOne(cacheDrive);
//                        this->hide();
//                        QLayoutItem* item;
//                        while ((item = this->vboxlayout->takeAt(0)) != NULL)
//                        {
//                            delete item->widget();
//                            delete item;
//                        }

//                        //hign = findList()->size()*50+30;

//                    });


                if(findGDriveList()->size() != 0)
                {
                    this->showNormal();
                    moveBottomNoBase();
                }

            }
          ui->centralWidget->show();
          interfaceHideTime->setTimerType(Qt::PreciseTimer);
          if(ui->centralWidget != NULL)
          {
              connect(interfaceHideTime, SIGNAL(timeout()), this, SLOT(on_Maininterface_hide()));
          }
          interfaceHideTime->start(2000);

      }


void MainWindow::on_Maininterface_hide()
{
    this->hide();
    interfaceHideTime->stop();
}

void MainWindow::moveBottomNoBase()
{
//    screen->availableGeometry();
//    screen->availableSize();
//    if(screen->availableGeometry().x() == screen->availableGeometry().y() && screen->availableSize().height() < screen->size().height())
//    {
//        qDebug()<<"the positon of panel is down";
//        this->move(screen->availableGeometry().x() + screen->size().width() -
//                   this->width() - DistanceToPanel,screen->availableGeometry().y() +
//                   screen->availableSize().height() - this->height() - DistanceToPanel);
//    }

//    if(screen->availableGeometry().x() < screen->availableGeometry().y() && screen->availableSize().height() < screen->size().height())
//    {
//        qDebug()<<"this position of panel is up";
//        this->move(screen->availableGeometry().x() + screen->size().width() -
//                   this->width() - DistanceToPanel,screen->availableGeometry().y());
//    }

//    if(screen->availableGeometry().x() > screen->availableGeometry().y() && screen->availableSize().width() < screen->size().width())
//    {
//        qDebug()<<"this position of panel is left";
//        this->move(screen->availableGeometry().x() + DistanceToPanel,screen->availableGeometry().y()
//                   + screen->availableSize().height() - this->height());
//    }

//    if(screen->availableGeometry().x() == screen->availableGeometry().y() && screen->availableSize().width() < screen->size().width())
//    {
//        qDebug()<<"this position of panel is right";
//        this->move(screen->availableGeometry().x() + screen->availableSize().width() -
//                   DistanceToPanel - this->width(),screen->availableGeometry().y() +
//                   screen->availableSize().height() - (this->height())*(DistanceToPanel - 1));
//    }
    int position=0;
    int panelSize=0;
    if(QGSettings::isSchemaInstalled(QString("org.ukui.panel.settings").toLocal8Bit()))
    {
        QGSettings* gsetting=new QGSettings(QString("org.ukui.panel.settings").toLocal8Bit());
        if(gsetting->keys().contains(QString("panelposition")))
            position=gsetting->get("panelposition").toInt();
        else
            position=0;
        if(gsetting->keys().contains(QString("panelsize")))
            panelSize=gsetting->get("panelsize").toInt();
        else
            panelSize=SmallPanelSize;
    }
    else
    {
        position=0;
        panelSize=SmallPanelSize;
    }

    int x=QApplication::primaryScreen()->geometry().x();
    int y=QApplication::primaryScreen()->geometry().y();

    if(position==0)
        this->setGeometry(QRect(x + QApplication::primaryScreen()->geometry().width()-this->width(),y+QApplication::primaryScreen()->geometry().height()-panelSize-this->height(),this->width(),this->height()));
    else if(position==1)
        this->setGeometry(QRect(x + QApplication::primaryScreen()->geometry().width()-this->width(),y+panelSize,this->width(),this->height()));  // Style::minw,Style::minh the width and the height of the interface  which you want to show
    else if(position==2)
        this->setGeometry(QRect(x+panelSize,y + QApplication::primaryScreen()->geometry().height() - this->height(),this->width(),this->height()));
    else
        this->setGeometry(QRect(x+QApplication::primaryScreen()->geometry().width()-panelSize-this->width(),y + QApplication::primaryScreen()->geometry().height() - this->height(),this->width(),this->height()));
}

/*
 * determine how to open the maininterface,if trigger is 0,the main interface show when we inset USB
 * device directly,if trigger is 1,we show main interface by clicking the systray icon.
*/
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(triggerType == 0)
    {
        if(event->type() == QEvent::Enter)
        {
            disconnect(interfaceHideTime, SIGNAL(timeout()), this, SLOT(on_Maininterface_hide()));
            this->show();
        }

        if(event->type() == QEvent::Leave)
        {
            connect(interfaceHideTime, SIGNAL(timeout()), this, SLOT(on_Maininterface_hide()));
            interfaceHideTime->start(2000);
        }
    }

    if(triggerType == 1){}

    if (obj == this)
    {
        if (event->type() == QEvent::WindowDeactivate && !(this->isHidden()))
        {
            this->hide();
            return true;
        }
//        else if (event->type() == QEvent::ActionChanged)
//        {
//            if (this->isHidden())
//            {
//                MainWindow::isShow = false;
//            }
//            else
//            {
//                MainWindow::isShow = true;
//            }
//        }
    }
    if (!isActiveWindow())
    {
        activateWindow();
    }
    return false;
}



//new a gsettings object to get the information of the opacity of the window
void MainWindow::initTransparentState()
{
//    if (QGSettings::isSchemaInstalled("org.ukui.control-center.personalise")) {
//        m_transparency_gsettings = new QGSettings("org.ukui.control-center.personalise");
//    }
    const QByteArray idtrans(THEME_QT_TRANS);

    if(QGSettings::isSchemaInstalled(idtrans))
    {
        m_transparency_gsettings = new QGSettings(idtrans);
    }
}

//use gsettings to get the opacity
void MainWindow::getTransparentData()
{
    if (!m_transparency_gsettings)
    {
       m_transparency = 0.95;
       return;
    }

    QStringList keys = m_transparency_gsettings->keys();
    if (keys.contains("transparency"))
    {
        qDebug()<<"m_transparency hasn't got over";
        qDebug()<<"m_transparency"<<m_transparency;
        qDebug()<<m_transparency_gsettings->get("transparency").toDouble()<<"real transparency";
        qDebug()<<"1111111111111111111111111111111";
        m_transparency = m_transparency_gsettings->get("transparency").toDouble();
        qDebug()<<"m_transpatrnccy has got over";
    }
//    m_transparency = m_transparency_gsettings->get("transparency").toDouble();
}
