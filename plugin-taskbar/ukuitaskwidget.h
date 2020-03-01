/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
 *
 * Copyright: 2019 Tianjin KYLIN Information Technology Co., Ltd. *
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#ifndef UKUITASKWIDGET_H
#define UKUITASKWIDGET_H

#include <QProxyStyle>
#include "../panel/iukuipanel.h"
//#include <QWinThumbnailToolBar>
#include <QtX11Extras/qtx11extrasversion.h>
#include <QLabel>
#include <QVBoxLayout>
#include <QStyle>

class QPainter;
class QPalette;
class QMimeData;
class UKUITaskGroup;
class UKUITaskBar;
class UKUITaskCloseButton;

class UKUITaskWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(Qt::Corner origin READ origin WRITE setOrigin)

public:
    explicit UKUITaskWidget(const WId window, UKUITaskBar * taskBar, QWidget *parent = 0);
    virtual ~UKUITaskWidget();

    bool isApplicationHidden() const;
    bool isApplicationActive() const;
    WId windowId() const { return mWindow; }

    bool hasUrgencyHint() const { return mUrgencyHint; }
    void setUrgencyHint(bool set);

    bool isOnDesktop(int desktop) const;
    bool isOnCurrentScreen() const;
    bool isMinimized() const;
    void updateText();

    Qt::Corner origin() const;
    virtual void setAutoRotation(bool value, IUKUIPanel::Position position);

    UKUITaskBar * parentTaskBar() const {return mParentTaskBar;}

    void refreshIconGeometry(QRect const & geom);
    static QString mimeDataFormat() { return QLatin1String("ukui/UKUITaskWidget"); }
    /*! \return true if this buttom received DragEnter event (and no DragLeave event yet)
     * */
    bool hasDragAndDropHover() const;
    void setThumbNail(QPixmap _pixmap);
    void setTitle();

public slots:
    void raiseApplication();
    void minimizeApplication();
    void maximizeApplication();
    void deMaximizeApplication();
    void shadeApplication();
    void unShadeApplication();
    void closeApplication();
    void moveApplicationToDesktop();
    void moveApplication();
    void resizeApplication();
    void setApplicationLayer();

    void setOrigin(Qt::Corner);

    void updateIcon();

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent * event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);

    void setWindowId(WId wid) {mWindow = wid;}
    virtual QMimeData * mimeData();
    static bool sDraggging;

    inline IUKUIPanelPlugin * plugin() const { return mPlugin; }

private:
    WId mWindow;
    bool mUrgencyHint;
    QPoint mDragStartPosition;
    Qt::Corner mOrigin;
    QPixmap mPixmap;
    bool mDrawPixmap;
    UKUITaskBar * mParentTaskBar;
    IUKUIPanelPlugin * mPlugin;
    QLabel *mTitleLabel;
    QLabel *mThumbnailLabel;
    QLabel *mAppIcon;
    UKUITaskCloseButton *mCloseBtn;
    QVBoxLayout *mVWindowsLayout;
    QHBoxLayout *mTopBarLayout;

    // Timer for when draggind something into a button (the button's window
    // must be activated so that the use can continue dragging to the window
    QTimer * mDNDTimer;
    enum TaskWidgetStatus{NORMAL, HOVER, PRESS};
    TaskWidgetStatus status;
    bool taskWidgetPress; //按钮左键是否按下

private slots:
    void activateWithDraggable();

signals:
    void dropped(QObject * dragSource, QPoint const & pos);
    void dragging(QObject * dragSource, QPoint const & pos);
};

typedef QHash<WId,UKUITaskWidget*> UKUITaskButtonHash;

#endif // UKUITASKWIDGET_H
