#include <qfile.h>
#include <qdatastream.h>

int main()
{
    QFile file("file");
    file.open(IO_WriteOnly);

    QDataStream out(&file);

    out << (Q_UINT32)0;

    file.close();

    file.open(IO_ReadOnly);

    QDataStream in(&file);

    Q_UINT32 data;

    in >> data;

    qDebug("%i", data);

    file.close();
}

