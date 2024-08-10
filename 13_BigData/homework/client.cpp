#include <filesystem>

/**
 * В этом файле находится клиентский код, который использует наш MapReduce фреймворк.
 * Этот код знает о том, какую задачу мы решаем.
 * Задача этого кода - верно написать мапер, редьюсер, запустить mapreduce задачу, обработать
 * результат. Задача - найти минимальную длину префикса, который позволяет однозначно
 * идентифицировать строку в файле. Задача не решается в одну mapreduce задачу. Нужно делать
 * несколько запусков.
 *
 * Как предлагаю делать я:
 * Выделяем первые буквы слов (в мапере), решаем для них задачу "определить, есть ли в них повторы".
 * Если не прокатило, повторяем процедуру, выделяя первые две буквы.
 * И т.д. В итоге найдём длину префикса, который однозначно определяет строку.
 *
 * Здесь описано то, как я примерно решал бы задачу, это не руководство к действию, а просто
 * пояснение к основному тексту задания. Вы можете поступать по-своему (не как я описываю), задание
 * творческое! Можете делать так, как написано, если считаете, что это хорошо.
 */

#include <iostream>
#include <fstream>

#include "client.h"

int main() {
    std::ifstream fin("emails.txt");
    std::filesystem::path output("./Testing/");
    std::cout << "min prefix: " << calc_min_prefix(fin, output, 3, 2) << "\n";

    return 0;
}