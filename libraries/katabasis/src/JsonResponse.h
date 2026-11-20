#pragma once

#include <QVariantMap>

class QByteArray;

namespace Katabasis {

QVariantMap parseJsonResponse(const QByteArray& data);

}   
