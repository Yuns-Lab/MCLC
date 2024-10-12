#ifndef MCLC_LOGGER_H
#define MCLC_LOGGER_H

#include <QString>
#include <QObject>

namespace MCLC {

    class Logger : public QObject {
    private:
        static Logger logger;
    public:
        Logger();
        Logger& getInstance();
        Logger& info(QString str);
        Logger& debug(QString str);
        Logger& warn(QString str);
        Logger& error(QString str);
    };

} // MCLC

#endif //MCLC_LOGGER_H
