#pragma once
#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include "data_structures.h"
#include "config.h"
#include <string>
#include <vector>
#include <map>

// Главная функция загрузки всех данных игры
bool loadGameData(GameData& gameData);

// Вспомогательные функции для загрузки каждого типа данных (вызываются из loadGameData)
// Они будут объявлены здесь, а определены в file_parser.cpp
bool loadPlayerStatsFromFile(PlayerStats& playerStats, const std::string& filename);
bool loadMonstersFromFile(std::vector<MonsterData>& monsters, const std::string& filename);
bool loadItemsFromFile(std::vector<ItemData>& items, const std::string& filename);
bool loadSpellsFromFile(std::vector<SpellData>& spells, const std::string& filename);
bool loadBooksFromFile(std::map<std::string, std::string>& bookContents, const std::string& filename);
bool loadPreparationActionsFromFile(std::vector<PreparationActionData>& actions, const std::string& filename);

#endif // FILE_PARSER_H