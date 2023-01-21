#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqltablemodel.h>
#include <QTableView>
#include <QDebug>
#include <string>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QKeyEvent>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /// @brief Конструктор, в котором происходит подключение и открытие базы данных, подготовка базы данных к работе и вывод базы данных с дефолтными параметрами фильтрации
    
    MainWindow(QWidget *parent = nullptr);
    /// @brief Диструктор, в котором освобождается занятая память
    ~MainWindow();

private:
    /// @brief Поле, хранящее указатель на экземпляр, отвечающий за графический интерфейс приложения
    Ui::MainWindow *ui;
    /// @brief Поле для настройки соединения с базой данных
    QSqlDatabase db;
    /// @brief Поле, хранящее указатель на экземпляр и предоставляющий средства для обработки операторов SQL и управления ими
    QSqlQuery *query;
    /// @brief Поле, хранящее указатель на экземпляр и предоставляющее модель данных, доступную только для чтения, для наборов результатов SQL
    QSqlQueryModel *qmodel;
    /// @brief Отчистка предидущей база дынных
    void clear_db();
    /// @brief Создание пустой базы данных
    void create_db();
    /// @brief Заполнение базы данных
    void fill_db();
    /// @brief ID аудитории на которую нажал пользователь 
    int currentId;
    QString curentAud;
protected:
    /// @brief Обработка нажатия клавиши "Enter" для вывода базы данных с установленными фильтрами
  
    void keyPressEvent(QKeyEvent *e) override;

private slots:
    /// @brief Вывод базы данных с установленными  фильтрами
    void applyFilters();
    /// @brief Отчистка фильтров базы данных
    void clearFilters();
    /// @brief Вывод базы данных с установленными фильтрами после нажатия кнопки "Найти"
    void on_btnSetFilters_clicked();
    /// @brief Обработка изменения значений слайдера
    void on_sdrCapacity_valueChanged();
    /// @brief Добавленияаудитории в дефолтными параметрами в базу данных
    void on_btnAddAudit_clicked();
    /// @brief Удаление выбранном аудитории из базы данных
    void on_btnRemoveAudit_clicked();
    /// @brief Подстановка параметров пользователя из полей в базу данных
    void on_pushButtonSaveInfo_clicked();
    /// @brief Вывод всех аудиторий из базы данных (отчищает филтр поиска)
    void on_pushButtonShowAll_clicked();
    /// @brief Обработка нажатий пользователя по таблице с аудиториями
    void on_tableView_clicked(const QModelIndex &index);

};
#endif // MAINWINDOW_H
