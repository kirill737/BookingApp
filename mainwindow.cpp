#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
//ПОДГОТОВКА БАЗЫ ДАННЫХ
    ui->setupUi(this);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./bookingAppDB.db");
    db.open();
    query = new QSqlQuery(db);
    qmodel = new QSqlQueryModel();
    clearFilters();
    currentId = -1;
    applyFilters();
}   

MainWindow::~MainWindow()
{
    delete ui;
}   

void MainWindow::on_sdrCapacity_valueChanged() {
//ЗНАЧЕНИЕ МЕНЯЕТСЯ ПРИ СДВИГЕ СЛАЙДЕРА
    int a = ui->sdrCapacity->value();
    QString text_a = QString::number(a);
    ui->lineEdit->setText(text_a);
}
void MainWindow::applyFilters(){
    // Фефолтные параметры сортировки
    QString finalRequest = "";
    QString filter = "", filter_corpus = "", filter_type = "", filter_cap = "", filter_equip = "", filter_search = "";
    QString order = "", columnToSort = "id";
    
    bool revSortMode = false;
    int combIndex = ui->comboBox->currentIndex();

    // Фильтр оборудования
    if (ui->cbxBoard->isChecked()) { filter_equip += "(equipment LIKE '%доска%')"; }
    if (ui->cbxComp->isChecked()) {
        if (filter_equip != "") filter_equip += " AND ";
        filter_equip += "(equipment LIKE '%компьютер%')";
    }
    if (ui->cbxProjector->isChecked()) {
        if (filter_equip != "") filter_equip += " AND ";
        filter_equip += "(equipment LIKE '%проектор%')";
    }

    // Фильтр корпуса
    if (ui->checkBox_build_A->isChecked()){
        if (filter_corpus != "") filter_corpus += "OR ";
        filter_corpus += "building = 'А'";
    }
    if (ui->checkBox_build_B->isChecked()){
        if (filter_corpus != "") filter_corpus += "OR ";
        filter_corpus += "building = 'Б'";
    }
    if (ui->checkBox_build_K->isChecked()){
        if (filter_corpus != "") filter_corpus += "OR ";
        filter_corpus += "building = 'К'";
    }
    if (ui->checkBox_build_L->isChecked()){
        if (filter_corpus != "") filter_corpus += "OR ";
        filter_corpus += "building = 'Л'";
    }
    if (ui->checkBox_build_G->isChecked()){
        if (filter_corpus != "") filter_corpus += "OR ";
        filter_corpus += "building = 'Г'";
    }
    
    // Фильтр типа аудитории
    if (ui->radioButton_Lab->isChecked()) filter_type = "(type LIKE '%лаборантская%')";
    if (ui->radioButton_Lec->isChecked()) filter_type = "(type LIKE '%лекционная%')";
    if (ui->radioButton_Pract->isChecked()) filter_type = "(type LIKE '%практическая%')";
    if (ui->radioButton_Any->isChecked()) filter_type = "";

    // Фильтр вместительности
    int a = ui->sdrCapacity->value();
    QString text_a = QString::number(a);
    ui->lineEdit->setText(text_a);
    filter_cap += "(capacity >=";
    filter_cap += text_a;
    filter_cap += ")";
    // Фильтр окна поиска
    QString text_search = ui->leSearch->text();
    if (text_search != "") {
        filter_search += "(full_num LIKE '%";
        for (int i = 0; i < text_search.length(); i++) {
            filter_search += text_search[i];
            if (text_search[i].isLetter()) filter_search += "%";
        }
        filter_search += "%') ";
    }

    // Определения порядка сортировки
    if (ui->checkBox_revSort->isChecked()) revSortMode = true;
    else revSortMode = false;
    switch (combIndex)
    {
    case 0:
        columnToSort = "full_num ";
        break;
    case 1:
        columnToSort = "capacity ";
        break;
    default:
        break;
    }

    // Подстановка всех данных в итоговый фильтр
    filter += filter_equip;
    if (filter_corpus != "") {
        if (filter != "") filter += " And ";
        filter+= "(";
        filter_corpus += ")";
        filter += filter_corpus;
    }
    if (filter_type != "") {
        if (filter != "") filter += " And ";
        filter += filter_type;
    }
    if (filter_cap != "") {
        if (filter != "") filter+= " And ";
        filter += filter_cap;
    }
    if (filter_search != "") {
        filter += " AND ";
        filter += filter_search; 
    }
    
    // Настрояка сортировки
    order += columnToSort;
    if (revSortMode)
        order += "DESC ";

    // Составление финального запроса
    finalRequest = "SELECT full_num AS 'Аудитория', type AS 'Тип', capacity AS 'Вместимость', equipment AS 'Оборудование', status AS 'Cостояние' FROM audiences_info ";
    finalRequest += "WHERE ";
    finalRequest += filter;
    finalRequest += " ORDER BY ";
    finalRequest += order;

    // Вывод отфильтрованных данных
    qmodel->setQuery(finalRequest);
    ui->tableView->setModel(qmodel);
    ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
}
void MainWindow::clearFilters(){
    // обнуляет фильтры
    ui->radioButton_Any->setChecked(true);
    ui->sdrCapacity->setValue(15);
    ui->lineEdit->setText("15");
    ui->leSearch->setText("");
    ui->cbxBoard->setChecked(false);
    ui->cbxComp->setChecked(false);
    ui->cbxProjector->setChecked(false);
    ui->checkBox_build_A->setChecked(false);
    ui->checkBox_build_B->setChecked(false);
    ui->checkBox_build_L->setChecked(false);
    ui->checkBox_build_G->setChecked(false);
    ui->checkBox_build_K->setChecked(false);
    ui->checkBox_revSort->setChecked(false);

    ui->btnRemoveAudit->setEnabled(false);
    ui->pushButtonSaveInfo->setEnabled(false);
    ui->lineEdit_valAudBuild->setText("");
    ui->lineEdit_valAudNumber->setText("");
    ui->lineEdit_valCap->setText("");
    ui->lineEdit_valEquip->setText("");
    ui->lineEdit_valType->setText("");
}
void MainWindow::on_btnSetFilters_clicked(){
    applyFilters();
}
void MainWindow::on_btnAddAudit_clicked(){
    QString valAud = "", valAudBuilding = "", valAudNumber = "", valType = "", valCap = "", valEquip = "", valStatus = "";
    int statusIndex = 0;
    

//ПОЛУЧЕНИЕ ИСХОДНЫХ ДАННЫХ ИЗ ТАБЛИЦЫ
    valAudNumber = ui->lineEdit_valAudNumber->text();
    valAudBuilding = ui->lineEdit_valAudBuild->text();
    valType = ui->lineEdit_valType->text();
    valCap = ui->lineEdit_valCap->text();
    valEquip = ui->lineEdit_valEquip->text();
    valStatus = ui->comboBox_valStatus->currentText();
    valAud = valAudBuilding + "-" + valAudNumber;
    bool flag_aud = true, flag = true, noError = true;;
    if ((valAudNumber == "") || (valAudBuilding == "")) {flag_aud = false, noError = false;}
    QString getIdRequest = "SELECT id FROM audiences_info WHERE (full_num = '" + valAud + "')";
    query->exec(getIdRequest);
    while (query->next()) {
                flag = false;
                noError = false;
            }
//ДОБАВЛЯЕТ АУДИТОРИЮ С ВЫСТАВЛЕННЫМИ ПАРАМЕТРАМИ
    if (!flag_aud) (QMessageBox::information(this, "Ошибка при добавление аудитории", "Не указано корпус или номер аудитории."));
    else if (!flag) (QMessageBox::information(this, "Ошибка при добавление аудитории", "Аудитория с номером " + valAud + " уже существует. Если хотите обновить информацию о ней, нажмите кнопку 'Сохранить данные.'"));
    else if (noError) {
        QString execQuery = "INSERT INTO audiences_info (full_num, capacity, type, equipment, status, building, number) VALUES";
        execQuery += "('" + valAud + "', '" + valCap + "', '" + valType + "', '" + valEquip + "', '" + valStatus + "', '" + valAudBuilding + "', '" + valAudNumber + "')";
        query->exec(execQuery);
    }
    
//ВЫВОД ОТФИЛЬТРОВАННЫХ ДАННЫХ
    if (noError) {
        clearFilters();
        ui->pushButtonSaveInfo->setEnabled(false);
        qmodel->setQuery("SELECT full_num AS 'Аудитория', type AS 'Тип', capacity AS 'Вместимость', equipment AS 'Оборудование', status AS 'Cостояние' FROM audiences_info ");
        ui->tableView->setModel(qmodel);
        ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
        ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
        ui->tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    }
    
}
void MainWindow::on_pushButtonShowAll_clicked() {
//ВЫВОДИТ СПИСОК ВСЕХ АУДИТОРИЯ БЕЗ ФИЛЬТРОВ
    qmodel->setQuery("SELECT full_num AS 'Аудитория', type AS 'Тип', capacity AS 'Вместимость', equipment AS 'Оборудование', status AS 'Cостояние' FROM audiences_info");
    ui->tableView->setModel(qmodel);
    clearFilters();
    ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
}
void MainWindow::on_btnRemoveAudit_clicked(){
//УДАЛЯЕТ ВЫБРАННУЮ СТРОКУ
    QString deleteRequest = "DELETE FROM audiences_info WHERE (id = " ;
    deleteRequest += QString::number(currentId);
    deleteRequest +=");";
    qmodel->setQuery(deleteRequest);
//УМЕНЬШАЕТ ID У СТРОК ПОСЛЕ УДАЛЁННОЙ
    QString updateRequest = "UPDATE audiences_info SET id = (id - 1) WHERE (id > ";
    updateRequest += QString::number(currentId);
    updateRequest += ")";
    if (currentId != -1) {
        qmodel->setQuery(updateRequest);
    }
//ОБНОВЛЯЕТ АВТОИНКРЕМЕНТ 
    updateRequest = "UPDATE SQLITE_SEQUENCE SET seq = ";
    updateRequest += QString::number(currentId - 1);
    updateRequest += " WHERE (name = 'audiences_info')";
    qmodel->setQuery(updateRequest);
    currentId = -1;
    applyFilters();

    ui->btnRemoveAudit->setEnabled(false);
    ui->pushButtonSaveInfo->setEnabled(false);
    ui->lineEdit_valAudBuild->setText("");
    ui->lineEdit_valAudNumber->setText("");
    ui->lineEdit_valCap->setText("");
    ui->lineEdit_valEquip->setText("");
    ui->lineEdit_valType->setText("");
}
void MainWindow::on_tableView_clicked(const QModelIndex &index) {
    QString valAud = "", valAudBuilding = "", valAudNumber = "", valType = "", valCap = "", valEquip = "", valStatus = "";
    int statusIndex = 0;
    ui->pushButtonSaveInfo->setEnabled(true);
    ui->btnRemoveAudit->setEnabled(true);
//ПОЛУЧЕНИЕ ИСХОДНЫХ ДАННЫХ ИЗ ТАБЛИЦЫ
    valAud = ui->tableView->model()->data(index.sibling(index.row(), 0)).toString();
    valType = ui->tableView->model()->data(index.sibling(index.row(), 1)).toString();
    valCap = ui->tableView->model()->data(index.sibling(index.row(), 2)).toString();
    valEquip = ui->tableView->model()->data(index.sibling(index.row(), 3)).toString();
    valStatus = ui->tableView->model()->data(index.sibling(index.row(), 4)).toString();
    valAudBuilding = valAud[0];
    for (int i = 2; i < valAud.length(); i++ ) {
        valAudNumber += valAud[i];
    }
//ПОЛУЧЕНИЕ ID ВЫБРАННОЙ СТРОКИ
    QString getIdRequest = "SELECT id FROM audiences_info WHERE (full_num = '" + valAud + "')";
    query->exec(getIdRequest);
    while (query->next()) {
                currentId = query->value("id").toInt();
            }
//ПОДСТАНОВКА ЗНАЧЕНИЙ ИЗ ВЫБРАННОЙ СТРОКИ
    ui->lineEdit_valAudBuild->setText(valAudBuilding);
    ui->lineEdit_valAudNumber->setText(valAudNumber);
    ui->lineEdit_valType->setText(valType);
    ui->lineEdit_valCap->setText(valCap);
    ui->lineEdit_valEquip->setText(valEquip);
    ui->lineEdit_valAudBuild->setText(valAudBuilding);
    ui->lineEdit_valAudNumber->setText(valAudNumber);
    if (valStatus == "доступна") statusIndex = 0;
    if (valStatus == "в ремонте") statusIndex = 1;
    if (valStatus == "не доступна") statusIndex = 2;
    ui->comboBox_valStatus->setCurrentIndex(statusIndex);    
}
void MainWindow::on_pushButtonSaveInfo_clicked() {
    QString valAud = "", valAudBuilding = "", valAudNumber = "", valType = "", valCap = "", valEquip = "", valStatus = "";
    int statusIndex = 0;
    bool flag_num = true, flag_build = true, noError = true, flag = true;
    valAudBuilding = ui->lineEdit_valAudBuild->text();
    valAudNumber = ui->lineEdit_valAudNumber->text();
    valAud = valAudBuilding + "-" + valAudNumber;
    valType = ui->lineEdit_valType->text();
    valCap = ui->lineEdit_valCap->text();
    valEquip = ui->lineEdit_valEquip->text();
    valStatus = ui->comboBox_valStatus->currentText();
    if (valAudNumber == "") {flag_num = false, noError = false;}
    if (valAudBuilding == "") {flag_build = false, noError = false;}
    QString getIdRequest = "SELECT id FROM audiences_info WHERE (full_num = '" + valAud + "')";
    query->exec(getIdRequest);
    while (query->next()) {
                flag = false;
                noError = false;
            }
    if (!flag_build && !flag_num) (QMessageBox::information(this, "Ошибка при изменение данных", "Не указан номер и корпус аудитории."));
    else {
        if (!flag_num) (QMessageBox::information(this, "Ошибка при изменение данных", "Не указан номер аудитории."));
        if (!flag_build) (QMessageBox::information(this, "Ошибка при изменение данных", "Не указан корпус аудитории."));
    }
    
    if (!flag) (QMessageBox::information(this, "Ошибка при изменение данных", "Аудитория с номером " + valAud + " уже существует."));
    
    if (noError) {
        QString updateRequest = "UPDATE audiences_info SET (full_num, capacity, type, equipment, status, building, number) = ('";
        updateRequest += valAud;
        updateRequest += "', '";
        updateRequest += valCap;
        updateRequest += "', ' ";
        updateRequest += valType;
        updateRequest += "', '";
        updateRequest += valEquip;
        updateRequest += "', '";
        updateRequest += valStatus;
        updateRequest += "', '";
        updateRequest += valAudBuilding;
        updateRequest += "', '";
        updateRequest += valAudNumber;
        updateRequest += "')";
        updateRequest += " WHERE (id = ";
        updateRequest += QString::number(currentId);
        updateRequest += ")";
        qmodel->setQuery(updateRequest);
        ui->tableView->setModel(qmodel);
        applyFilters();
        ui->pushButtonSaveInfo->setEnabled(false);
        ui->lineEdit_valAudBuild->setText("");
        ui->lineEdit_valAudNumber->setText("");
        ui->lineEdit_valCap->setText("");
        ui->lineEdit_valEquip->setText("");
        ui->lineEdit_valType->setText("");
    }
}
void MainWindow::keyPressEvent(QKeyEvent *e) {
    if(e->key() == Qt::Key_Return) {
        applyFilters();
    }
}