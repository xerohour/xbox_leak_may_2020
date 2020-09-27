//-----------------------------------------------------------------------------
// File: Text.cpp
//
// Desc: All text for LoadSave sample in single place to simplify localization
//
// Hist: 04.10.01 - Added for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Text.h"





//-----------------------------------------------------------------------------
// Order of text matches XC_LANGUAGE_XXX #definitions in Xbox.h
//-----------------------------------------------------------------------------

const WCHAR* const strSAVE_FAILED[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"Unable to save game.\n"
    L"Press A to continue",

    L"",

    L"Spiel konnte nicht\n"
    L"gespeichert werden", // DE

    L"Impossible d'enregistrer\n"
    L"la partie", // FR

    L"Fallo al guardar\n"
    L"la partida", // ES

    L"Impossibile salvare\n"
    L"la partita", // IT
};

const WCHAR* const strGAME_SAVED[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Game Saved",
    L"",
    L"Spiel wurde gespeichert", // DE
    L"La partie est enregistr" L"\xE9" L"e", // FR
    L"Partida guardada", // ES
    L"Partita salvata", // IT
};

const WCHAR* const strLOAD_FAILED[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"Unable to load game.\n"
    L"Press A to continue",

    L"",

    L"Spiel konnte nicht\n"
    L"geladen werden", // DE

    L"Impossible de charger\n"
    L"la partie", // FR

    L"Fallo al cargar\n"
    L"la partida", // ES

    L"Impossibile caricare\n"
    L"la partita", // IT
};

const WCHAR* const strGAME_LOADED[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Game Loaded",
    L"",
    L"Spiel wurde geladen", // DE
    L"La partie est charg" L"\xE9" L"e", // FR
    L"Partida cargada", // ES
    L"Partita caricata", // IT
};

const WCHAR* const strNO_ROOM_MU[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"The memory unit does not\n"
    L"have enough free blocks\n"
    L"for this saved game.",

    L"",

    L"F" L"\xFC" L"r das gespeicherte Spiel\n"
    L"sind nicht ausreichend freie\n"
    L"Blocks auf der Speicherkarte\n"
    L"vorhanden", // DE

    L"Il n'y a pas assez de blocs\n"
    L"libres dans l'unit" L"\xE9" L" de\n"
    L"m" L"\xE9" L"moire pour enregistrer\n"
    L"cette partie", // FR

    L"La unidad de memoria no\n"
    L"dispone de suficientes\n"
    L"bloques para la partida\n"
    L"guardada", // ES

    L"I blocchi disponibili\n"
    L"sull'unit" L"\xE0" L" di memoria non sono\n"
    L"sufficienti per questa\n"
    L"partita salvata", // IT
};

const WCHAR* const strNO_ROOM_HD[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"The hard disk does not\n"
    L"have enough free blocks\n"
    L"for this saved game.",

    L"",

    // Need to be updated for HD translations ...
    L"F" L"\xFC" L"r das gespeicherte Spiel\n"
    L"sind nicht ausreichend freie\n"
    L"Blocks auf der Speicherkarte\n"
    L"vorhanden", // DE

    L"Il n'y a pas assez de blocs\n"
    L"libres dans l'unit" L"\xE9" L" de\n"
    L"m" L"\xE9" L"moire pour enregistrer\n"
    L"cette partie", // FR

    L"La unidad de memoria no\n"
    L"dispone de suficientes\n"
    L"bloques para la partida\n"
    L"guardada", // ES

    L"I blocchi disponibili\n"
    L"sull'unit" L"\xE0" L" di memoria non sono\n"
    L"sufficienti per questa\n"
    L"partita salvata", // IT
};

const WCHAR* const strNO_ROOM_MU_PLZ_FREE[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"The memory unit does not\n"
    L"have enough free blocks for\n"
    L"this saved game.\n"
    L"%lu blocks must be free.",

    L"",

    L"F" L"\xFC" L"r das gespeicherte Spiel\n"
    L"sind nicht ausreichend freie\n"
    L"Blocks auf der Speicherkarte\n"
    L"vorhanden. %lu Blocks\n"
    L"m" L"\xFC" L"ssen verf" L"\xFC" L"gbar sein.", // DE

    L"Il n'y a pas assez de blocs\n"
    L"libres dans l'unit" L"\xE9" L" de\n"
    L"m" L"\xE9" L"moire pour enregistrer\n"
    L"cette partie. Vous devez\n"
    L"lib" L"\xE9" L"rer %lu blocs.", // FR

    L"La unidad de memoria no\n"
    L"dispone de suficientes\n"
    L"bloques para la partida\n"
    L"guardada. Se necesitan\n"
    L"%lu bloques disponibles.", // ES

    L"I blocchi disponibili\n"
    L"sull'unit" L"\xE0" L" di memoria non sono\n"
    L"sufficienti per questa partita\n"
    L"salvata. Sono necessari %lu\n"
    L"blocchi disponibili.", // IT
};

const WCHAR* const strNO_ROOM_HD_PLZ_FREE[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"The hard disk does not\n"
    L"have enough free blocks for\n"
    L"this saved game.\n"
    L"%lu blocks must be free.",

    L"",

    // Need to be updated for HD translations ...
    L"F" L"\xFC" L"r das gespeicherte Spiel\n"
    L"sind nicht ausreichend freie\n"
    L"Blocks auf der Speicherkarte\n"
    L"vorhanden. %lu Blocks\n"
    L"m" L"\xFC" L"ssen verf" L"\xFC" L"gbar sein.", // DE

    L"Il n'y a pas assez de blocs\n"
    L"libres dans l'unit" L"\xE9" L" de\n"
    L"m" L"\xE9" L"moire pour enregistrer\n"
    L"cette partie. Vous devez\n"
    L"lib" L"\xE9" L"rer %lu blocs.", // FR

    L"La unidad de memoria no\n"
    L"dispone de suficientes\n"
    L"bloques para la partida\n"
    L"guardada. Se necesitan\n"
    L"%lu bloques disponibles.", // ES

    L"I blocchi disponibili\n"
    L"sull'unit" L"\xE0" L" di memoria non sono\n"
    L"sufficienti per questa partita\n"
    L"salvata. Sono necessari %lu\n"
    L"blocchi disponibili.", // IT
};

const WCHAR* const strNO_SAVES[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"No saved games",

    L"",

    L"Keine gespeicherten\n"
    L"Spiele vorhanden", // DE

    L"Il n'y a pas de\n"
    L"partie enregistr" L"\xE9" L"e", // FR

    L"No hay partidas guardadas", // ES

    L"Nessuna partita salvata", // IT
};

const WCHAR* const strLOADING[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"Loading Game",

    L"",

    L"Spiel wird geladen", // DE

    L"Chargement de la\n"
    L"partie", // FR

    L"Cargando partida", // ES

    L"Caricamento partita in\n"
    L"corso", // IT
};

const WCHAR* const strSAVING_MU[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"Saving game to memory unit\n%s.\n",

    L"",

    L"Saving game to memory unit\n%s.\n", // DE
    L"Saving game to memory unit\n%s.\n", // FR
    L"Saving game to memory unit\n%s.\n", // ES
    L"Saving game to memory unit\n%s.\n", // IT
};

const WCHAR* const strSAVING[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"Saving Game",

    L"",

    L"Spiel wird gespeichert", // DE

    L"Enregistrement de la\n"
    L"partie", // FR

    L"Guardando partida", // ES

    L"Salvataggio partita in\n"
    L"corso", // IT
};

const WCHAR* const strDO_NOT_REMOVE_MU[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"Please don't remove memory\n"
    L"units or disconnect controllers.",

    L"",

    L"Entfernen Sie nicht die\n"
    L"Speicherkarte oder die\n"
    L"Controller.", // DE

    L"Ne supprimez pas\n"
    L"d'unit" L"\xE9" L"s de m" L"\xE9" L"moire ni ne\n"
    L"d" L"\xE9" L"connectez les contr" L"\xF4" L"leurs.", // FR

    L"No retire las unidades\n"
    L"de memoria ni desconecte\n"
    L"los controladores.", // ES

    L"Non rimuovere le unit" L"\xE0" L"\n"
    L"di memoria o disconnettere\n"
    L"i controller.\n", // IT
};

const WCHAR* const strDO_NOT_POWEROFF[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L". Please don't\n"
    L"turn off your Xbox console.\n",

    L"",

    L".\n"
    L"Schalten Sie die\n"
    L"Xbox nicht aus.", // DE

    L". N'" L"\xE9" L"teignez\n"
    L"pas votre Xbox.", // FR

    L". No\n"
    L"apague la Xbox.", // ES

    L". Non spegnere\n"
    L"la Xbox.", // IT
};

const WCHAR* const strSAVE_GAME[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Save Game",
    L"",
    L"Spiel speichern", // DE
    L"Enregistrer la partie", // FR
    L"Guardar partida", // ES
    L"Salva partita", // IT
};

const WCHAR* const strLOAD_GAME[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Load Game",
    L"",
    L"Spiel laden", // DE
    L"Charger la partie", // FR
    L"Cargar partida", // ES
    L"Carica partita", // IT
};

const WCHAR* const strILLUS_GRAPHICS[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Sample graphics. Don't use in your game",
    L"",
    L"Grafiken nur zur Illustration", // DE
    L"Exemples de graphiques uniquement", // FR
    L"Gr" L"\xE1" L"ficos s" L"\xF3" L"lo de muestra", // ES
    L"Grafica solo dimostrativa", // IT
};

const WCHAR* const strFORMAT_DEVICE[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"Total blocks\n%hs\n"
    L"Available blocks\n%hs",

    L"",

    L"Gesamtgr" L"\xF6" L"\xDF" L"e\n%hs\n"
    L"Verf" L"\xFC" L"gbare Gr" L"\xF6" L"\xDF" L"e\n%hs", // DE

    L"Blocs totale\n%hs\n"
    L"Blocs disponible\n%hs", // FR

    L"Bloques total\n%hs\n"
    L"Bloques disponible\n%hs", // ES

    L"Blocchi totale\n%hs\n"
    L"Blocchi disponibile\n%hs", // IT
};

const WCHAR* const strCHOOSE_LOAD[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"choose memory device to load game",
    L"",
    L"W" L"\xE4" L"hlen Sie ein Speichermedium zum Laden des Spiels", // DE
    L"Choisissez une unit" L"\xE9" L" de m" L"\xE9" L"moire pour charger la partie", // FR
    L"Elija un dispositivo de memoria para cargar la partida", // ES
    L"Scegliere la periferica di memoria da cui caricare il gioco", // IT
};

const WCHAR* const strCHOOSE_SAVE[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"choose memory device to save game",
    L"",
    L"W" L"\xE4" L"hlen Sie ein Speichermedium zum Speichern des Spiels", // DE
    L"Choisissez une unit" L"\xE9" L" de m" L"\xE9" L"moire pour enregistrer la partie", // FR
    L"Elija un dispositivo de memoria para guardar la partida", // ES
    L"Scegliere la periferica di memoria su cui salvare il gioco", // IT
};

const WCHAR* const strTODAY[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Today",
    L"",
    L"Heute", // DE
    L"Aujourd'hui", // FR
    L"Hoy", // ES
    L"Oggi", // IT
};

const WCHAR* const strNOW[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Now",
    L"",
    L"Jetzt", // DE
    L"Maintenant", // FR
    L"Ahora", // ES
    L"Ora", // IT
};

const WCHAR* const strFORMAT_GAME[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"Name: %s\nBlocks: %hs\n"
    L"Date: %s\nTime: %s",

    L"",

    L"Name: %s\nBlocks: %hs\n"
    L"Datum: %s\nZeit: %s", // DE

    L"Nom: %s\nBlocs: %hs\n"
    L"Date: %s\nHeure: %s", // FR

    L"Nombre: %s\nBloques: %hs\n"
    L"Fecha: %s\nHora: %s", // ES

    L"Nome: %s\nBlocchi: %hs\n"
    L"Date: %s\nOra: %s", // IT
};

const WCHAR* const strEMPTY_SPACE[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Empty Space",
    L"",
    L"Freier Speicherplatz", // DE
    L"Espace disponible", // FR
    L"Espacio vac" L"\xED" L"o", // ES
    L"Spazio disponibile", // IT
};

const WCHAR* const strA_SELECT[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"select",
    L"",
    L"ausw" L"\xE4" L"hlen", // DE
    L"s" L"\xE9" L"lectionner", // FR               
    L"seleccionar", // ES
    L"seleziona", // IT
};

const WCHAR* const strB_BACK[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"back",
    L"",
    L"zur" L"\xFC" L"ck", // DE
    L"retour", // FR
    L"atr" L"\xE1" L"s", // ES
    L"indietro", // IT
};

const WCHAR* const strY_DELETE[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"delete",
    L"",
    L"l" L"\xF6" L"schen", // DE
    L"supprimer", // FR
    L"eliminar", // ES
    L"elimina", // IT
};

const WCHAR* const strOVERWRITE[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"Overwrite saved game?",

    L"",

    L"Gespeichertes Spiel\n"
    L"\xFC" L"berschreiben?", // DE

    L"Ecraser la partie\n"
    L"enregistr" L"\xE9" L"e?", // FR

    L"\xBF" L"Desea sobrescribir\n"
    L"la partida guardada?", // ES

    L"Sovrascrivere la\n"
    L"partita salvata?", // IT
};

const WCHAR* const strYES[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Yes",
    L"",
    L"Ja", // DE
    L"Oui", // FR
    L"S" L"\xED" L"", // ES
    L"S" L"\xEC" L"", // IT
};

const WCHAR* const strNO[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"No",
    L"",
    L"Nein", // DE
    L"Non", // FR
    L"No", // ES
    L"No", // IT
};

const WCHAR* const strDELETE[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"Delete saved game?",

    L"",

    L"Gespeichertes Spiel\n"
    L"l" L"\xF6" L"schen?", // DE

    L"Supprimer la partie\n"
    L"enregistr" L"\xE9" L"e?", // FR

    L"\xBF" L"Desea eliminar la\n"
    L"partida guardada?", // ES

    L"Eliminare la partita\n"
    L"salvata?", // IT
};

const WCHAR* const strLOADING_GAME_LIST[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"Loading game list",

    L"",

    L"Spieleliste wird geladen", // DE

    L"Chargement de la liste\n"
    L"des parties", // FR

    L"Cargando lista\n"
    L"de partidas", // ES

    L"Caricamento elenco\n"
    L"partite", // IT
};

const WCHAR* const strMAX_SAVED_GAMES[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"There is a maximum of\n"
    L"4096 saved games.",

    L"",

    L"There is a maximum of\n"
    L"4096 saved games.", // DE

    L"There is a maximum of\n"
    L"4096 saved games.", // FR

    L"There is a maximum of\n"
    L"4096 saved games.", // ES

    L"There is a maximum of\n"
    L"4096 saved games.", // IT
};

const WCHAR* const strMU_UNUSABLE[ XC_LANGUAGE_MAX ] = 
{
    L"",

	L"This memory unit is unusable",
    
    L"",

    L"Diese Speicherkarte ist\n"
    L"m" L"\xF6" L"glicherweise besch" L"\xE4" L"digt", // DE

    L"Cette unit" L"\xE9" L" de m" L"\xE9" L"moire\n"
    L"est peut-" L"\xEA" L"tre endommag" L"\xE9" L"e", // FR

    L"Es posible que esta\n"
    L"unidad de memoria\n"
    L"est" L"\xE9" L" da" L"\xF1" L"ada", // ES

    L"Questa unit" L"\xE0" L" di memoria\n"
    L"potrebbe essere\n"
    L"danneggiata", // IT
};

const WCHAR* const strMU_FULL[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"This memory unit is full",
    L"This memory unit is full", // JP
    L"Diese Speicher-Einheit ist voll", // DE
    L"La Carte m" L"\x00E9"  L"moire est pleine", // FR
    L"Esta unidad de memoria est" L"\x00E1" L"llena", // ES
    L"Questa memory unit " L"\x00E8" L"piena", // IT
};

const WCHAR* const strUNUSABLE_MU_NAME[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Unusable Xbox MU",
    L"",
    L"Xbox-SE besch" L"\xE4" L"digt", // DE
    L"CM Xbox endommag" L"\xE9" L"e", // FR
    L"UM de la Xbox da" L"\xF1" L"ada", // ES
    L"UM Xbox danneggiata", // IT
};

const WCHAR* const strFULL_MU_NAME[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Full Xbox MU",
    L"",
    L"Xbox-SE voll.", // DE
    L"CM Xbox pleine", // FR
    L"UM Xbox llena", // ES
    L"MU Xbox piena", // IT
};

const WCHAR* const strMU_REMOVED[ XC_LANGUAGE_MAX ] =
{
    L"",

    L"%s\nwas removed during use.\n"
    L"%s failed.\n"
    L"Press A to continue.",

    L"",

    L"%s\nwurde w" L"\xE4" L"hrend Benutzung\n"
    L"entfernt. %s ist\n"
    L"fehlgeschlagen. Weiter\n"
    L"mit A-Taste.", // DE

    L"%s\na " L"\xE9" L"t" L"\xE9" L" retir" L"\xE9" L"e de mani" L"\xE8" L"re\n"
    L"intempestive. %s\n"
    L"a " L"\xE9" L"chou" L"\xE9" L". Appuyer sur A\n"
    L"pour continuer.", // FR

    L"%s\nse ha extra" L"\xED" L"do mientras\n"
    L"estaba en uso. Error\n"
    L"de %s. Presione A\n"
    L"para continuar.", // ES

    L"%s\nrimosso durante l'uso.\n"
    L"Errore di %s. Premere\n"
    L"A per continuare.", // IT
};

const WCHAR* const strACTION_SAVE[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Save",
    L"",
    L"Speichern", // DE
    L"Sauvegarder", // FR
    L"Guardar", // ES
    L"Salva", // IT
};

const WCHAR* const strACTION_LOAD[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Load",
    L"",
    L"Laden", // DE
    L"Charger", // FR
    L"Cargar", // ES
    L"Carica", // IT
};

const CHAR* const strMAX_BLOCKS[ XC_LANGUAGE_MAX ] =
{
    "",
    "50,000+",
    "50,000+", // JP
    "50.000+", // DE
    "50 000+", // FR
    "50.000+", // ES
    "50.000+", // IT
};

const WCHAR* const strXHD[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Xbox Hard Disk",
    L"Xbox Hard Disk", // JP
    L"Xbox Festplatte", // DE
    L"Xbox Disque", // FR
    L"Xbox Disco", // ES
    L"Xbox Disco Rigido", // IT
};
