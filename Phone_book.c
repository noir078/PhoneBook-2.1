
#include <stdio.h>
#include <string.h>
#include <locale.h>

#define MAX_CONTACTS 100
#define FILE_NAME "contacts.txt"
#define NAME_SIZE 256
#define TEXT_SIZE 512
#define PHONE_SIZE 100

typedef struct {
    char surname[NAME_SIZE];
    char name[NAME_SIZE];
    char patronymic[NAME_SIZE];
    char workplace[TEXT_SIZE];
    char position[TEXT_SIZE];
    char phone[PHONE_SIZE];
    char email[TEXT_SIZE];
    char social[TEXT_SIZE];
    char messenger[TEXT_SIZE];
} Contact;

Contact contacts[MAX_CONTACTS];
int count = 0;

int fileEnabled = 0;
int fileDamaged = 0;


// =====================================
// ЧТЕНИЕ СТРОКИ
// =====================================

int readLine(const char *prompt, char *buffer, size_t size) {
    size_t len;
    int ch;

    if (size == 0) {
        return 0;
    }

    printf("\n%s", prompt);
    fflush(stdout);

    if (fgets(buffer, (int)size, stdin) == NULL) {
        return 0;
    }

    len = strlen(buffer);

    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[--len] = '\0';
    } else {
        ch = getchar();

        if (ch != '\n' && ch != EOF) {
            while (ch != '\n' && ch != EOF) {
                ch = getchar();
            }

            buffer[0] = '\0';
            printf("Слишком длинный ввод! Попробуйте ещё раз.\n");
            return -1;
        }
    }

    len = strlen(buffer);

    if (len > 0 && buffer[len - 1] == '\r') {
        buffer[len - 1] = '\0';
    }

    return 1;
}


// =====================================
// УДАЛЕНИЕ ЦИФР
// =====================================

void removeDigits(char *str) {
    size_t j = 0;

    for (size_t i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') {
            str[j++] = str[i];
        }
    }

    str[j] = '\0';
}


// =====================================
// ПРОВЕРКА ПУСТОЙ СТРОКИ
// =====================================

int isEmpty(const char *str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (str[i] != ' ' &&
            str[i] != '\t' &&
            str[i] != '\r') {
            return 0;
        }
    }

    return 1;
}


// =====================================
// ОБЯЗАТЕЛЬНЫЙ ВВОД ИМЕНИ И ФАМИЛИИ
// =====================================

int readRequiredName(const char *prompt,
                     char *buffer, size_t size) {

    while (1) {
        int status = readLine(prompt, buffer, size);

        if (status == 0) {
            return 0;
        }

        if (status == -1) {
            continue;
        }

        removeDigits(buffer);

        size_t start = 0;

        while (buffer[start] == ' ' ||
               buffer[start] == '\t') {
            start++;
        }

        if (start > 0) {
            memmove(buffer, buffer + start,
                    strlen(buffer + start) + 1);
        }

        if (!isEmpty(buffer)) {
            return 1;
        }

        printf("Поле обязательно! Введите значение повторно.\n");
    }
}


// =====================================
// ЧТЕНИЕ СТРОКИ ИЗ ФАЙЛА
// =====================================

int readFileLine(FILE *file, char *buffer, size_t size) {
    size_t i = 0;
    int ch;
    int received = 0;

    if (size == 0) {
        return -1;
    }

    while ((ch = fgetc(file)) != '\n' && ch != EOF) {
        received = 1;

        if (i < size - 1) {
            buffer[i++] = (char)ch;
        } else {
            while (ch != '\n' && ch != EOF) {
                ch = fgetc(file);
            }

            return -1;
        }
    }

    if (!received && ch == EOF) {
        buffer[0] = '\0';
        return 0;
    }

    if (i > 0 && buffer[i - 1] == '\r') {
        i--;
    }

    buffer[i] = '\0';

    return 1;
}


// =====================================
// СОХРАНЕНИЕ КОНТАКТОВ
// =====================================

int saveContacts(void) {
    FILE *file;

    if (fileDamaged) {
        printf("Сохранение отменено: файл повреждён.\n");
        printf("Проверьте contacts.txt перед сохранением.\n");
        return 0;
    }

    file = fopen(FILE_NAME, "w");

    if (file == NULL) {
        perror("Ошибка создания contacts.txt");
        return 0;
    }

    for (int i = 0; i < count; i++) {
        if (fprintf(file, "%s\n", contacts[i].surname) < 0 ||
            fprintf(file, "%s\n", contacts[i].name) < 0 ||
            fprintf(file, "%s\n", contacts[i].patronymic) < 0 ||
            fprintf(file, "%s\n", contacts[i].workplace) < 0 ||
            fprintf(file, "%s\n", contacts[i].position) < 0 ||
            fprintf(file, "%s\n", contacts[i].phone) < 0 ||
            fprintf(file, "%s\n", contacts[i].email) < 0 ||
            fprintf(file, "%s\n", contacts[i].social) < 0 ||
            fprintf(file, "%s\n", contacts[i].messenger) < 0) {

            printf("Ошибка записи файла!\n");
            fclose(file);
            return 0;
        }
    }

    if (fclose(file) != 0) {
        perror("Ошибка закрытия файла");
        return 0;
    }

    fileEnabled = 1;

    printf("Контакты успешно сохранены в %s!\n", FILE_NAME);

    return 1;
}


// =====================================
// АВТОМАТИЧЕСКОЕ СОХРАНЕНИЕ
// =====================================

void autoSave(void) {
    if (fileEnabled) {
        saveContacts();
    }
}


// =====================================
// ЗАГРУЗКА КОНТАКТОВ
// =====================================

void loadContacts(void) {
    FILE *file = fopen(FILE_NAME, "r");

    count = 0;
    fileDamaged = 0;
    fileEnabled = 0;

    if (file == NULL) {
        printf("Файл ещё не создан.\n");
        return;
    }

    while (count < MAX_CONTACTS) {
        Contact c = {0};

        int status = readFileLine(
            file, c.surname, sizeof(c.surname)
        );

        if (status == 0) {
            break;
        }

        if (status != 1 ||
            readFileLine(file, c.name, sizeof(c.name)) != 1 ||
            readFileLine(file, c.patronymic,
                         sizeof(c.patronymic)) != 1 ||
            readFileLine(file, c.workplace,
                         sizeof(c.workplace)) != 1 ||
            readFileLine(file, c.position,
                         sizeof(c.position)) != 1 ||
            readFileLine(file, c.phone,
                         sizeof(c.phone)) != 1 ||
            readFileLine(file, c.email,
                         sizeof(c.email)) != 1 ||
            readFileLine(file, c.social,
                         sizeof(c.social)) != 1 ||
            readFileLine(file, c.messenger,
                         sizeof(c.messenger)) != 1) {

            fileDamaged = 1;
            break;
        }

        if (isEmpty(c.surname) || isEmpty(c.name)) {
            fileDamaged = 1;
            break;
        }

        contacts[count++] = c;
    }

    // Проверяем, нет ли контактов сверх лимита.
    if (!fileDamaged && count == MAX_CONTACTS) {
        int ch = fgetc(file);

        if (ch != EOF) {
            fileDamaged = 1;
        }
    }

    fclose(file);

    if (fileDamaged) {
        fileEnabled = 0;

        printf("Обнаружена ошибка в файле контактов.\n");
        printf("Автоматическое сохранение отключено.\n");
        printf("Проверьте contacts.txt перед сохранением.\n");

        return;
    }

    fileEnabled = 1;

    printf("Загружено контактов: %d\n", count);
}


// =====================================
// ДОБАВЛЕНИЕ КОНТАКТА
// =====================================

void addContact(void) {
    Contact c = {0};

    if (count >= MAX_CONTACTS) {
        printf("Телефонная книга заполнена!\n");
        return;
    }

    if (!readRequiredName("Фамилия: ",
                          c.surname, sizeof(c.surname))) return;

    if (!readRequiredName("Имя: ",
                          c.name, sizeof(c.name))) return;

    if (readLine("Отчество: ", c.patronymic,
                 sizeof(c.patronymic)) != 1) return;

    if (readLine("Место работы: ", c.workplace,
                 sizeof(c.workplace)) != 1) return;

    if (readLine("Должность: ", c.position,
                 sizeof(c.position)) != 1) return;

    if (readLine("Телефон: ", c.phone,
                 sizeof(c.phone)) != 1) return;

    if (readLine("Email: ", c.email,
                 sizeof(c.email)) != 1) return;

    if (readLine("Соцсеть: ", c.social,
                 sizeof(c.social)) != 1) return;

    if (readLine("Мессенджер: ", c.messenger,
                 sizeof(c.messenger)) != 1) return;

    contacts[count] = c;
    count++;

    printf("Контакт добавлен!\n");

    autoSave();
}


// =====================================
// ВЫВОД ОДНОГО КОНТАКТА
// =====================================

void printContact(const Contact *c, int index) {
    printf("\n--- Контакт %d ---\n", index + 1);

    printf("Фамилия: %s\n", c->surname);
    printf("Имя: %s\n", c->name);
    printf("Отчество: %s\n", c->patronymic);
    printf("Место работы: %s\n", c->workplace);
    printf("Должность: %s\n", c->position);
    printf("Телефон: %s\n", c->phone);
    printf("Email: %s\n", c->email);
    printf("Соцсеть: %s\n", c->social);
    printf("Мессенджер: %s\n", c->messenger);
}


// =====================================
// ПРОСМОТР КОНТАКТОВ
// =====================================

void showContacts(void) {
    if (count == 0) {
        printf("Телефонная книга пуста.\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        printContact(&contacts[i], i);
    }
}


// =====================================
// ПОКАЗ ВСЕХ ЗАПОЛНЕННЫХ ДАННЫХ
// ПРИ СОВПАДЕНИИ ФАМИЛИЙ
// =====================================

void printMatchingContact(const Contact *c, int number) {
    printf("\n--- Совпадение %d ---\n", number);

    if (!isEmpty(c->surname))
        printf("Фамилия: %s\n", c->surname);

    if (!isEmpty(c->name))
        printf("Имя: %s\n", c->name);

    if (!isEmpty(c->patronymic))
        printf("Отчество: %s\n", c->patronymic);

    if (!isEmpty(c->workplace))
        printf("Место работы: %s\n", c->workplace);

    if (!isEmpty(c->position))
        printf("Должность: %s\n", c->position);

    if (!isEmpty(c->phone))
        printf("Телефон: %s\n", c->phone);

    if (!isEmpty(c->email))
        printf("Email: %s\n", c->email);

    if (!isEmpty(c->social))
        printf("Соцсеть: %s\n", c->social);

    if (!isEmpty(c->messenger))
        printf("Мессенджер: %s\n", c->messenger);
}


// =====================================
// ПОИСК КОНТАКТА ПО ФАМИЛИИ
// =====================================

int findContact(void) {
    char surname[NAME_SIZE];
    char input[32];
    char extra;

    int matches[MAX_CONTACTS];
    int matchCount = 0;
    int choice;

    if (readRequiredName("Введите фамилию: ",
                         surname, sizeof(surname)) != 1) {
        return -1;
    }

    // Ищем все контакты с введённой фамилией.
    for (int i = 0; i < count; i++) {
        if (strcmp(contacts[i].surname, surname) == 0) {
            matches[matchCount++] = i;
        }
    }

    if (matchCount == 0) {
        printf("Контакт не найден.\n");
        return -1;
    }

    // Если найден только один контакт.
    if (matchCount == 1) {
        return matches[0];
    }

    printf("\nНайдено несколько контактов.\n");
    printf("Показаны все заполненные данные:\n");

    for (int i = 0; i < matchCount; i++) {
        printMatchingContact(&contacts[matches[i]], i + 1);
    }

    // Выбираем нужного человека.
    while (1) {
        int status = readLine(
            "\nВведите номер нужного контакта (0 - отмена): ",
            input, sizeof(input)
        );

        if (status != 1) {
            return -1;
        }

        if (sscanf(input, "%d %c", &choice, &extra) != 1) {
            printf("Введите правильный номер!\n");
            continue;
        }

        if (choice == 0) {
            return -1;
        }

        if (choice < 1 || choice > matchCount) {
            printf("Такого контакта нет!\n");
            continue;
        }

        return matches[choice - 1];
    }
}


// =====================================
// РЕДАКТИРОВАНИЕ ОБЫЧНЫХ ПОЛЕЙ
// =====================================

int editField(const char *prompt,
              char *field, size_t size) {

    char temp[TEXT_SIZE];

    int status = readLine(prompt, temp, sizeof(temp));

    if (status != 1) {
        return status;
    }

    if (temp[0] != '\0') {
        if (strlen(temp) >= size) {
            printf("Слишком длинное значение!\n");
            return -1;
        }

        snprintf(field, size, "%s", temp);
    }

    return 1;
}


// =====================================
// РЕДАКТИРОВАНИЕ ИМЕНИ И ФАМИЛИИ
// =====================================

int editName(const char *prompt,
             char *field, size_t size) {

    char temp[NAME_SIZE];

    while (1) {
        int status = readLine(prompt, temp, sizeof(temp));

        if (status == 0) {
            return 0;
        }

        if (status == -1) {
            continue;
        }

        // Пустой ввод оставляет старое значение.
        if (temp[0] == '\0') {
            return 1;
        }

        removeDigits(temp);

        if (isEmpty(temp)) {
            printf("Введите непустое имя или фамилию.\n");
            continue;
        }

        if (strlen(temp) >= size) {
            printf("Слишком длинное значение!\n");
            continue;
        }

        snprintf(field, size, "%s", temp);

        return 1;
    }
}


// =====================================
// РЕДАКТИРОВАНИЕ КОНТАКТА
// =====================================

void editContact(void) {
    int i = findContact();

    if (i == -1) {
        return;
    }

    Contact edited = contacts[i];

    printf("\nРедактирование выбранного контакта.\n");
    printf("Пустой ввод оставляет старое значение.\n");

    if (editName("Новая фамилия: ",
                 edited.surname, sizeof(edited.surname)) != 1) return;

    if (editName("Новое имя: ",
                 edited.name, sizeof(edited.name)) != 1) return;

    if (editField("Новое отчество: ",
                  edited.patronymic,
                  sizeof(edited.patronymic)) != 1) return;

    if (editField("Новое место работы: ",
                  edited.workplace,
                  sizeof(edited.workplace)) != 1) return;

    if (editField("Новая должность: ",
                  edited.position,
                  sizeof(edited.position)) != 1) return;

    if (editField("Новый телефон: ",
                  edited.phone,
                  sizeof(edited.phone)) != 1) return;

    if (editField("Новый email: ",
                  edited.email,
                  sizeof(edited.email)) != 1) return;

    if (editField("Новая соцсеть: ",
                  edited.social,
                  sizeof(edited.social)) != 1) return;

    if (editField("Новый мессенджер: ",
                  edited.messenger,
                  sizeof(edited.messenger)) != 1) return;

    if (isEmpty(edited.surname) || isEmpty(edited.name)) {
        printf("Фамилия и имя обязательны!\n");
        return;
    }

    contacts[i] = edited;

    printf("Контакт успешно изменён!\n");

    autoSave();
}


// =====================================
// УДАЛЕНИЕ КОНТАКТА
// =====================================

void deleteContact(void) {
    int i = findContact();

    if (i == -1) {
        return;
    }

    printf("\nВыбран контакт:\n");
    printContact(&contacts[i], i);

    char answer[16];

    if (readLine("Удалить этот контакт? (да/нет): ",
                 answer, sizeof(answer)) != 1) {
        return;
    }

    if (strcmp(answer, "да") != 0 &&
        strcmp(answer, "Да") != 0 &&
        strcmp(answer, "ДА") != 0) {
        printf("Удаление отменено.\n");
        return;
    }

    for (int j = i; j < count - 1; j++) {
        contacts[j] = contacts[j + 1];
    }

    count--;

    printf("Контакт удалён!\n");

    autoSave();
}


// =====================================
// ГЛАВНАЯ ФУНКЦИЯ
// =====================================

int main(void) {
    setlocale(LC_ALL, "");

    int choice;
    char input[32];
    char extra;

    // Загружаем контакты при запуске.
    loadContacts();

    // Если файла ещё нет, создаём пустой файл.
    if (!fileEnabled && !fileDamaged) {
        if (saveContacts()) {
            printf("Файл телефонной книги создан.\n");
        }
    }

    while (1) {
        printf("\n=== ТЕЛЕФОННАЯ КНИГА ===\n");
        printf("1. Добавить контакт\n");
        printf("2. Показать контакты\n");
        printf("3. Редактировать контакт\n");
        printf("4. Удалить контакт\n");
        printf("5. Сохранить в файл\n");
        printf("0. Выход\n");

        int status = readLine("Ваш выбор: ",
                              input, sizeof(input));

        if (status == 0) {
            break;
        }

        if (status == -1 ||
            sscanf(input, "%d %c", &choice, &extra) != 1) {
            printf("Введите номер пункта меню.\n");
            continue;
        }

        switch (choice) {
            case 1:
                addContact();
                break;

            case 2:
                showContacts();
                break;

            case 3:
                editContact();
                break;

            case 4:
                deleteContact();
                break;

            case 5:
                saveContacts();
                break;

            case 0:
                printf("Выход из программы.\n");
                return 0;

            default:
                printf("Неверный пункт меню.\n");
                break;
        }
    }

    return 0;
}
