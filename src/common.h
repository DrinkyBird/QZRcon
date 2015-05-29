#ifndef COMMON_H
#define COMMON_H

#define HAVE_QT5 (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))

#ifdef __cplusplus
#include <QtGui>
#include <QListView>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QHostInfo>
// Qt 5
#ifdef HAVE_QT5
#include <QListView>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QFontDialog>
#include <QSound>
#endif
#endif 

#ifdef _RELEASE
#define QT_NO_DEBUG_OUTPUT
#endif

#endif // COMMON_H
