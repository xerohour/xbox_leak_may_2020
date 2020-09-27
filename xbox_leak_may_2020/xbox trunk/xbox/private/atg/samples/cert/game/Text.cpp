//-----------------------------------------------------------------------------
// File: Text.cpp
//
// Desc: All text in single place to simplify localization
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Text.h"





//-----------------------------------------------------------------------------
// Order of text matches XC_LANGUAGE_XXX #definitions in Xbox.h
//-----------------------------------------------------------------------------

// TCR 3-25 Xbox Official Naming Standards
// TCR 3-27 Xbox Official Naming Localization

const WCHAR* const strGAME_NAME_FORMAT[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"%s drive %s", // EN
	L"%s " L"\x30C9" L"\x30E9" L"\x30A4" L"\x30D6" L" %s", //JP
    L"LW - %s %s", // DE
    L"Sauvegarde du %s %s", // FR
    L"Carrera de %s %s", // ES
    L"Gara di %s drive %s", // IT
};

const WCHAR* const strSUNDAY[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Sun.",
    L"\x65E5" L"\x66DC" L"\x65E5" L" = "  L"\x65E5", // JP
    L"So.", // DE
    L"dim.", // FR
    L"dom.", // ES
    L"dom.", // IT
};

const WCHAR* const strMONDAY[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Mon.",
    L"\x6708" L"\x66DC" L"\x65E5" L" = " L"\x6708", // JP
    L"Mo.", // DE
    L"cun.", // FR
    L"lun.", // ES
    L"lun.", // IT
};

const WCHAR* const strTUESDAY[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Tues",
    L"\x706B" L"\x66DC" L"\x65E5" L" = " L"\x706B", // JP
    L"Di.", // DE
    L"mar.", // FR
    L"mar.", // ES
    L"mar.", // IT
};

const WCHAR* const strWEDNESDAY[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Wed.",
    L"\x6C34" L"\x66DC" L"\x65E5" L" = " L"\x6C34", // JP
    L"mi.", // DE
    L"mer.", // FR
    L"mi" L"\xE9" L".", // ES
    L"mer.", // IT
};

const WCHAR* const strTHURSDAY[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Thurs",
    L"\x6728" L"\x66DC" L"\x65E5"  L" = " L"\x6728", // JP
    L"Do.", // DE
    L"jeu.", // FR
    L"jue.", // ES
    L"gio.", // IT
};

const WCHAR* const strFRIDAY[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Friday",
    L"\x91D1" L"\x66DC" L"\x65E5"  L" = " L"\x91D1", // JP
    L"Fr", // DE
    L"ven", // FR
    L"vie.", // ES
    L"ven.", // IT
};

const WCHAR* const strSATURDAY[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Sat",
    L"\x571F" L"\x66DC" L"\x65E5"  L" = " L"\x571F", // JP
    L"Sa", // DE
    L"sam.", // FR
    L"s" L"\xE1" L"b.", // ES
    L"sab.", // IT
};

// TCR 6-7 Lost Controller
const WCHAR* const strRECONNECT_CNTRLR[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Please reconnect the controller\nand press START to continue",

    L"\x30B3" L"\x30F3" L"\x30C8" L"\x30ED" L"\x30FC" L"\x30E9" L"\x3092" 
    L"\x63A5" L"\x7D9A" L"\x3057" L"\x76F4" L"\x3057" L"\x3001" 
    L"\nSTART" L"\x30DC" L"\x30BF" L"\x30F3" L"\x3092" L"\x62BC" L"\x3057" 
    L"\x3066" L"\x7D9A" L"\x3051" L"\x3066" L"\x304F" L"\x3060" 
    L"\x3055" L"\x3044" , // JP

    L"Bitte Controller wieder\nanschlie" L"\xDF" L"en", // DE
    L"Pour continuer, rebrancher la\nmanette et appuyer sur START.", // FR
    L"Vuelva a conectar el controlador y\n"
    L"presione START (inicio) para continuar.", // ES
    L"Ricollegare il controller e\npremere START per continuare.", // IT
};

const WCHAR* const strCONFIRM_QUIT[ XC_LANGUAGE_MAX ] =
{
    L"",

    L"Are you sure you\nwant to quit this race?\n\n"    
    L"Press A for Yes\n"
    L"Press B for No", // US

    L"\x3053" L"\x306E" L"\x30EC" L"\x30FC" L"\x30B9" L"\x3092" 
    L"\x7D42" L"\x4E86" L"\x3057" L"\x307E" L"\x3059" 
    L"\x304B" L"\x003F" L"\n\n"
    L"\x7D42" L"\x4E86" L"\x3059" L"\x308B" L" - A " 
    L"\x30DC" L"\x30BF" L"\x30F3" L"\n"
    L"\x7D9A" L"\x3051" L"\x308B" L" - B " 
    L"\x30DC" L"\x30BF" L"\x30F3",  //JP

    L"Dieses Rennen wirklich verlassen?\n\n"
    L"A - Taste f" L"\xFC" L"r JA dr" L"\xFC" L"cken\n"
    L"B - Taste f" L"\xFC" L"r NEIN dr" L"\xFC" L"cken",// DE

    L"Quitter cette course?\n\n"
    L"Oui: appuyer sur A\n"
    L"Non: appuyer sur B", // FR

    L"\xBF" L"Seguro que desea salir de la carrera?\n\n"
    L"Presione A si quiere\n"
    L"Presione B si no quiere", // ES

    L"Si " L"\xE8" L" sicuri di voler abbandonare questa gara?\n\n"
    L"Premere A per confermare\n"
    L"Premere B per rinunciare", // IT
};

const WCHAR* const strDEMO[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Demo",
    L"\x30C7" L"\x30E2", // JP
    L"Demo", // DE
    L"D" L"\xE9" L"mo", // FR
    L"Demo", // ES
    L"Demo", // IT
};

const WCHAR* const strRETURN_TO_MENU[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Press any button to return to the menu",

    L"\x30E1" L"\x30CB" L"\x30E5" L"\x30FC" L"\x306B" 
    L"\x623B" L"\x308B" L"\x306B" L"\x306F" L"\x3044" 
    L"\x305A" L"\x308C" L"\x304B" L"\x306E" L"\x30DC" 
    L"\x30BF" L"\x30F3" L"\x3092" L"\x62BC" L"\x3057" 
    L"\x3066" L"\x304F" L"\x3060" L"\x3055" L"\x3044", // JP

    L"Beliebige Taste zur R" L"\xFC" L"ckkehr", // DE
    L"Appuyer sur une touche pour revenir au menu", // FR
    L"Presione cualquier bot" L"\xF3" L"n para volver al men" L"\xFA", // ES
    L"Premere un pulsante per tornare al menu", // IT
};

const WCHAR* const strGAME_NAME[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Technical Certification Game",

    L"\x6280" L"\x8853" L"\x8A8D" L"\x8A3C" L"\x9805" 
    L"\x76EE" L"\x30C7" L"\x30E2" L"\x0020" L"\x30B2" 
    L"\x30FC" L"\x30E0" , // JP

    L"Spiel mit techn. Zertifikation", // DE
    L"Jeu de certification technique", // FR
    L"Partida de certificaci" L"\xF3" L"n t" L"\xE9" L"cnica", // ES
    L"Gioco con certificazione tecnica", // IT
};

const WCHAR* const strLOADINGX[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Loading...",
    L"\x8AAD" L"\x307F" L"\x8FBC" L"\x307F" L"\x4E2D" L"...", // JP
    L"Laden...", // DE
    L"Chargement en cours...", // FR
    L"Cargando...", // ES
    L"Caricamento in corso...", // IT
};

const WCHAR* const strFREE[ XC_LANGUAGE_MAX ] =
{
    L"",

    L"Your Xbox doesn't have enough\n"
    L"free blocks to save games.\n\n"
    L"Press A to continue without saving\n"
    L"or B to free at least %lu blocks.",

    L"\x30B2" L"\x30FC" L"\x30E0" L"\x306E" 
    L"\x4FDD" L"\x5B58" L"\x306B" L"\x5FC5" 
    L"\x8981" L"\x306A" L"\x7A7A" L"\x304D" 
    L"\x30D6" L"\x30ED" L"\x30C3" L"\x30AF" 
    L"\x304C" L"\x3042" L"\x308A" L"\x307E" 
    L"\x305B" L"\x3093" L"\x3002" L"\x0020" L"\n A " 
    L"\x30DC" L"\x30BF" L"\x30F3" L"\x3092" L"\x62BC" 
    L"\x3057" L"\x3066" L"\x4FDD" L"\x5B58" L"\x305B" 
    L"\x305A" L"\x306B" L"\x7D9A" L"\x3051" L"\x308B" 
    L"\x304B" L"\x3001" L"\n B " L"\x30DC" L"\x30BF" L"\x30F3" 
    L"\x3092" L"\x62BC" L"\x3057" L"\x3066" L"\x30D6" 
    L"\x30ED" L"\x30C3" L"\x30AF" L"\x3092" L"\x89E3" L"\x653E" 
    L"\x3057" L"\x3066" L"\x304F" L"\x3060" L"\x3055" 
    L"\x3044" L"\x3002", // JP

    L"Xbox-Festplatte hat nicht gen" L"\xFC" L"gend\n"
    L"freie Bl" L"\xF6" L"cke, um Spielst" L"\xE4" L"nde\n"
    L"zu speichern. A dr" L"\xFC" L"cken: Weiterspielen,\n"
    L"ohne zu speichern. B dr" L"\xFC" L"cken:\n"
    L"Bl" L"\xF6" L"cke frei machen.", // DE

    L"La Xbox ne comporte pas assez de blocs\n"
    L"libres pour sauvegarder les parties.\n"
    L"Appuyer sur A pour jouer sans sauvegarder.\n"
    L"Appuyer sur B pour lib" L"\xE9" L"rer des blocs.", // FR

    L"El disco de memoria de su Xbox no\n"
    L"dispone de suficientes bloques libres\n"
    L"para guardar partidas. Presione A para\n"
    L"continuar sin guardar o B para\n"
    L"liberar espacio.", // ES

    L"Sul disco rigido di Xbox non ci sono\n"
    L"blocchi liberi sufficienti per salvare\n"
    L"le partite. Premere A per continuare\n"
    L"senza salvare, oppure premere B per\n"
    L"liberare alcuni blocchi.", // IT
};

const WCHAR* const strMENU_START[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Start Race",
    L"\x30EC" L"\x30FC" L"\x30B9" L"\x306E" L"\x958B" L"\x59CB", // JP
    L"Rennen starten", // DE
    L"D" L"\xE9" L"marrer la course", // FR
    L"Comenzar carrera", // ES
    L"Inizia gara", // IT
};

const WCHAR* const strMENU_LOAD[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Load Saved Race",
    L"\x4FDD" L"\x5B58" L"\x3057" L"\x305F" L"\x30EC" L"\x30FC" 
    L"\x30B9" L"\x306E" L"\x8AAD" L"\x307F" L"\x8FBC" L"\x307F", // JP
    L"Gesp. Rennen starten", // DE
    L"Charger une course sauvegard" L"\xE9" L"e", // FR
    L"Cargar carrera guardada", // ES
    L"Carica gara salvata", // IT
};

const WCHAR* const strMENU_OPTIONS[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Options",
    L"\x30AA" L"\x30D7" L"\x30B7" L"\x30E7" L"\x30F3", // JP
    L"Options", // DE
    L"Options", // FR
    L"Opciones", // ES
    L"Opzioni", // IT
};

const WCHAR* const strMENU_RESUME[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Resume Race",
    L"\x30EC" L"\x30FC" L"\x30B9" L"\x306E" L"\x518D" L"\x958B", // JP
    L"Rennen weiterspielen", // DE
    L"Reprendre la course", // FR
    L"Continuar carrera", // ES
    L"Riprendi gara", // IT
};

const WCHAR* const strMENU_SAVE[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Save Race",
    L"\x30EC" L"\x30FC" L"\x30B9" L"\x306E" L"\x4FDD" L"\x5B58", // JP
    L"Rennen speichern", // DE
    L"Sauvegarder la course", // FR
    L"Guardar carrera", // ES
    L"Salva gara", // IT
};


const WCHAR* const strMENU_QUIT[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Quit Race",
    L"\x30EC" L"\x30FC" L"\x30B9" L"\x306E" L"\x7D42" L"\x4E86", // JP
    L"Rennen beenden", // DE
    L"Quitter la course", // FR
    L"Salir de carrera", // ES
    L"Abbandona gara", // IT
};

const WCHAR* const strMENU_VIBRATION[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Vibration",
    L"\x30B3" L"\x30F3" L"\x30C8" L"\x30ED" L"\x30FC" L"\x30E9" 
    L"\x306E" L"\x30D0" L"\x30A4" L"\x30D6" L"\x30EC" L"\x30FC" L"\x30B7" 
    L"\x30E7" L"\x30F3" L"\x0020" L"\x0020" L"\x632F" L"\x52D5" L"\x0029", // JP
    L"Controller-Vibration", // DE
    L"Vibration", // FR
    L"Funci" L"\xF3" L"n de vibraci" L"\xF3" L"n", // ES
    L"Vibrazione", // IT
};

const WCHAR* const strON[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"On",
    L"\x30AA" L"\x30F3", // JP
    L"Ein", // DE
    L"Oui", // FR
    L"Activar", // ES
    L"S" L"\xEC" L"", // IT
};

const WCHAR* const strOFF[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Off",
    L"\x30AA" L"\x30D5", // JP
    L"Aus", // DE
    L"Non", // FR
    L"Desactivar", // ES
    L"No", // IT
};

const WCHAR* const strMENU_MUSIC_VOLUME[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Music Volume",
    L"\x97F3" L"\x697D" L"\x306E" L"\x30DC" L"\x30EA" L"\x30E5" L"\x30FC" L"\x30E0", // JP
    L"Musik-Lautst" L"\x00E4" L"rke", // DE  
    L"Volume musique", // FR
    L"Volumen de m" L"\x00FA" L"sica", // ES
    L"Volume musica", // IT
};


const WCHAR* const strMENU_EFFECT_VOLUME[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Sound Effect Volume",
    L"\x52B9" L"\x679C" L"\x97F3" L"\x306E" L"\x30DC" L"\x30EA" L"\x30E5" L"\x30FC" L"\x30E0" , // JP
    L"Effekt-Lautst" L"\x00E4" L"rke", // DE  
    L"Volume effets son.", // FR
    L"Volumen de efectos", // ES
    L"Volume effetti", // IT
};

const WCHAR* const strMENU_SAVE_OPTIONS[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Save Options",
    L"\x30AA" L"\x30D7" L"\x30B7" L"\x30E7" L"\x30F3" L"\x306E" L"\x4FDD" L"\x5B58" , // JP
    L"Optionen speichern", // DE
    L"Sauvegarde des options", // FR
    L"Guardar opciones", // ES
    L"Salva opzioni", // IT
};

const WCHAR* const strMS_XBOX[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Microsoft Xbox",
    L"Microsoft Xbox", // JP
    L"Microsoft Xbox", // DE
    L"Microsoft Xbox", // FR
    L"Xbox de Microsoft", // ES
    L"Microsoft Xbox", // IT
};

const WCHAR* const strINTRO[ XC_LANGUAGE_MAX ] =
{
    L"",

    L"This sample shows a simple game that\n"
    L"meets Xbox Technical Certification\n"
    L"Requirements. Please do not lift the\n"
    L"graphics for use in your game.",

    L"\x3053" L"\x306E" L"\x30B5" L"\x30F3" L"\x30D7" 
    L"\x30EB" L"\x306F" L"\x3001" L" Xbox\n" L"\x306E" 
    L"\x6280" L"\x8853" L"\x9762" L"\x306E" L"\x691C" L"\x5B9A" 
    L"\x57FA" L"\x6E96" L"\x3092" L"\x6E80" L"\x305F" L"\x3057" 
    L"\x305F" L"\x7C21" L"\x5358" L"\x306A" L"\x30B2" L"\x30FC" 
    L"\x30E0" L"\x3067" L"\x3059" L"\x3002" L"\n" L"\x3053" L"\x306E" 
    L"\x30B2" L"\x30FC" L"\x30E0" L"\x3067" L"\x4F7F" L"\x7528" 
    L"\x3057" L"\x3066" L"\x3044" L"\x308B" L"\x753B" L"\x50CF" 
    L"\x306F" L"\x3001" L"\n" L"\x307B" L"\x304B" L"\x306E" L"\x30B2" 
    L"\x30FC" L"\x30E0" L"\x3067" L"\x306F" L"\x4F7F" L"\x7528" 
    L"\x3067" L"\x304D" L"\x307E" L"\x305B" L"\x3093" L"\x3002", // JP

    L"Dieses Beispiel ist ein einfaches Spiel,\n"
    L"das der techn. Zertifikation f" L"\xFC" L"r\n"
    L"die Xbox entspricht. Bitte die Grafik\n"
    L"nicht in Ihrem Spiel verwenden.",// DE

    L"Voici un jeu simple r" L"\xE9" L"pondant\n"
    L"aux Crit" L"\xE8" L"res de Certification\n"
    L"Technique Xbox. Merci de ne pas utiliser\n"
    L"ces graphismes dans votre jeu.",// FR

    L"\xC9" L"sta es una muestra de una partida\n"
    L"sencilla que re" L"\xFA" L"ne los requisitos\n"
    L"de certificaci" L"\xF3" L"n t" L"\xE9" L"cnica de\n"
    L"la Xbox. No utilice estos gr" L"\xE1" L"ficos\n"
    L"en la partida.", // ES

    L"Questo esempio mostra un semplice gioco che\n"
    L"soddisfa i requisiti di certificazione tecnica\n"
    L"per Xbox. Si prega di non riutilizzare la\n"
    L"grafica in altri giochi da voi sviluppati.", // IT
};

// TCR 6-6 Press Start
const WCHAR* const strPRESS_START[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Please press START to begin",
    L"START " L"\x30DC" L"\x30BF" L"\x30F3" L"\x3092" 
    L"\x62BC" L"\x3057" L"\x3066" L"\x958B" L"\x59CB" 
    L"\x3057" L"\x3066" L"\x304F" L"\x3060" L"\x3055" L"\x3044", // JP
    L"START dr" L"\xFC" L"cken, um zu beginnen", // DE
    L"Appuyer sur Start pour d" L"\xE9" L"marrer", // FR
    L"Presione START (inicio) para comenzar", // ES
    L"Premere START per iniziare", // IT
};

const WCHAR* const strSAVE_FAILED[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"Unable to save game.\n"
    L"Press A to continue",

    L"\x30B2" L"\x30FC" L"\x30E0" L"\x3092" L"\x4FDD" L"\x5B58" 
    L"\x3067" L"\x304D" L"\x307E" L"\x305B" L"\x3093" L"\x3002" 
    L"\n A " L"\x30DC" L"\x30BF" L"\x30F3" L"\x3092" L"\x62BC" 
    L"\x3057" L"\x3066" L"\x7D9A" L"\x3051" L"\x3066" L"\x304F" 
    L"\x3060" L"\x3055" L"\x3044",

    L"Spiel konnte nicht\n"
    L"gespeichert werden", // DE

    L"Impossible de sauvegarder\n"
    L"la partie. Appuyer sur A\n"
    L"pour continuer.",// FR

    L"Fallo al guardar la partida.\n"
    L"Presione A para continuar.", // ES

    L"Impossibile salvare la\n"
    L"partita. Premere A per\n"
    L"continuare.", // IT
};

const WCHAR* const strGAME_SAVED[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Game Saved",
    L"\x30B2" L"\x30FC" L"\x30E0" L"\x3092" L"\x4FDD" 
    L"\x5B58" L"\x3057" L"\x307E" L"\x3057" L"\x305F",
    L"Spiel wurde gespeichert", // DE
    L"Partie sauvegard" L"\xE9" L"e", // FR
    L"Partida guardada", // ES
    L"Partita salvata", // IT
};

// TCR 6-5 Content Protection Fault
const WCHAR* const strLOAD_FAILED[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"Unable to load game.\n"
    L"Press A to continue",

    L"", //need to get this string for JP

    L"Spiel konnte nicht\n"
    L"geladen werden", // DE

    L"Impossible de charger la\n"
    L"partie. Appuyer sur A pour\n"
    L"continuer.",// FR

    L"Fallo al cargar la partida.\n"
    L"Presione A para continuar.", // ES

    L"Impossibile caricare\n"
    L"la partita. Premere A\n"
    L"per continuare.", // IT
};

const WCHAR* const strGAME_LOADED[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Game Loaded",
    L"\x30B2" L"\x30FC" L"\x30E0" L"\x3092" L"\x8AAD" L"\x307F" 
    L"\x8FBC" L"\x307F" L"\x307E" L"\x3057" L"\x305F",
    L"Spiel wurde geladen", // DE
    L"Partie charg" L"\xE9" L"e", // FR
    L"Partida cargada", // ES
    L"Partita caricata", // IT
};

const WCHAR* const strNO_ROOM_MU[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"The memory unit does not\n"
    L"have enough free blocks\n"
    L"for this saved game.",

    L"\x4FDD" L"\x5B58" L"\x3057" L"\x305F" L"\x30B2" L"\x30FC" 
    L"\x30E0" L"\x3092" L"\x30D7" L"\x30EC" L"\x30A4" L"\x3059" 
    L"\x308B" L"\x306B" L"\x306F" L"\x3001" L"\n" L"\x30E1" L"\x30E2" 
    L"\x30EA" L"\x0020" L"\x30E6" L"\x30CB" L"\x30C3" L"\x30C8" 
    L"\x306E" L"\x7A7A" L"\x304D" L"\x30D6" L"\x30ED" L"\x30C3" 
    L"\x30AF" L"\x6570" L"\x304C" L"\x8DB3" L"\x308A" L"\x307E" 
    L"\x305B" L"\x3093" L"\x3002", //JP

    L"Die Speicher-Einheit hat nicht\n"
    L"gen" L"\xFC" L"gend freie Bl" L"\xF6" L"cke f" L"\xFC" L"r \n"
    L"diesen Spielstand.",// DE

    L"La Carte m" L"\xE9" L"moire ne\n"
    L"comporte pas assez de blocs\n"
    L"libres pour sauvegarder\n"
    L"la partie.",// FR

    L"La Unidad de Memoria no\n"
    L"dispone de suficientes\n"
    L"bloques libres para la\n"
    L"partida guardada.", // ES

    L"Sull'unit" L"\xE0" L" di memoria non\n"
    L"ci sono blocchi liberi sufficienti\n"
    L"per questo salvataggio.", // IT
};

const WCHAR* const strNO_ROOM_HD[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"The hard disk does not\n"
    L"have enough free blocks\n"
    L"for this saved game.",

    L"\x4FDD" L"\x5B58" L"\x3057" L"\x305F" L"\x30B2" L"\x30FC" 
    L"\x30E0" L"\x3092" L"\x30D7" L"\x30EC" L"\x30A4" L"\x3059" 
    L"\x308B" L"\x306B" L"\x306F" L"\x3001" L"\n" L"\x30CF" L"\x30FC" 
    L"\x30C9" L"\x0020" L"\x30C7" L"\x30A3" L"\x30B9" L"\x30AF" 
    L"\x306E" L"\x7A7A" L"\x304D" L"\x30D6" L"\x30ED" L"\x30C3" 
    L"\x30AF" L"\x6570" L"\x304C" L"\x8DB3" L"\x308A" L"\x307E" 
    L"\x305B" L"\x3093" L"\x3002",

    L"Die Festplatte hat nicht\n"
    L"gen" L"\xFC" L"gend\n freie Bl" L"\xF6" L"cke f" L"\xFC" L"r\n"
    L"diesen Spielstand.", // DE

    L"Le disque dur ne comporte\n"
    L"pas assez de blocs libres\n"
    L"pour sauvegarder la partie.", // FR

    L"El disco de memoria no dispone\n"
    L"de suficientes bloques libres\n"
    L"para la partida guardada", // ES

    L"Sul disco rigido non ci sono\n"
    L"blocchi liberi sufficienti\n"
    L"per questo salvataggio.", // IT
};

const WCHAR* const strNO_ROOM_MU_PLZ_FREE[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"The memory unit does not\n"
    L"have enough free blocks for\n"
    L"this saved game.\n"
    L"%lu blocks must be free.",

    L"\x3053" L"\x306E" L"\x30E1" L"\x30E2" L"\x30EA" L"\x0020" 
    L"\x30E6" L"\x30CB" L"\x30C3" L"\x30C8" L"\x306B" L"\x306F" 
    L"\x3001" L"\x5FC5" L"\x8981" L"\x306A" L"\x7A7A" L"\x304D" 
    L"\x5BB9" L"\x91CF" L"\x304C" L"\x3042" L"\x308A" L"\x307E" 
    L"\x305B" L"\x3093" L"\x3002" L"\n %lu " L"\x30D6" L"\x30ED" 
    L"\x30C3" L"\x30AF" L"\x306E" L"\x7A7A" L"\x304D" L"\x5BB9" 
    L"\x91CF" L"\x3092" L"\x78BA" L"\x4FDD" L"\x3057" L"\x3066" 
    L"\x304F" L"\x3060" L"\x3055" L"\x3044" L"\x3002" L"\n A " 
    L"\x30DC" L"\x30BF" L"\x30F3" L"\x3092" L"\x62BC" L"\x3057" 
    L"\x3066" L"\x7D9A" L"\x3051" L"\x3066" L"\x304F" L"\x3060" 
    L"\x3055" L"\x3044" L"\x3002", //JP

    L"Auf dieser Speicher-Einheit ist\n"
    L"nicht gen" L"\xFC" L"gend Platz. Bitte\n"
    L"%lu Bl" L"\xF6" L"cke frei machen.",// DE

    L"La Carte m" L"\xE9" L"moire ne\n"
    L"comporte pas assez de blocs\n"
    L"libres. Il faut effacer\n"
    L"%lu blocs.", // FR

    L"La Unidad de Memoria no\n"
    L"dispone de suficientes\n"
    L"bloques libres. Se necesitan\n"
    L"%lu bloques. Presione\n"
    L"A para continuar.", // ES

    L"Sull'unit" L"\xE0" L" di memoria non\n"
    L"ci sono blocchi liberi sufficienti\n"
    L"per questo salvataggio.\n"
    L"Occorre liberare %lu blocchi.", // IT
};

const WCHAR* const strNO_ROOM_HD_PLZ_FREE[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"The hard disk does not\n"
    L"have enough free blocks for\n"
    L"this saved game.\n"
    L"%lu blocks must be free.",

    L"\x3053" L"\x306E" L"\x30CF" L"\x30FC" L"\x30C9" L"\x0020" L"\x30C7" 
    L"\x30A3" L"\x30B9" L"\x30AF" L"\x306B" L"\x306F" L"\x3001" L"\x5FC5" 
    L"\x8981" L"\x306A" L"\x7A7A" L"\x304D" L"\x5BB9" L"\x91CF" L"\x304C" 
    L"\x3042" L"\x308A" L"\x307E" L"\x305B" L"\x3093" L"\x3002" L"\n %lu " 
    L"\x30D6" L"\x30ED" L"\x30C3" L"\x30AF" L"\x306E" L"\x7A7A" L"\x304D" 
    L"\x5BB9" L"\x91CF" L"\x3092" L"\x78BA" L"\x4FDD" L"\x3057" L"\x3066" 
    L"\x304F" L"\x3060" L"\x3055" L"\x3044" L"\x3002" L"\n A " L"\x30DC" 
    L"\x30BF" L"\x30F3" L"\x3092" L"\x62BC" L"\x3057" L"\x3066" L"\x7D9A"  
    L"\x3051" L"\x3066" L"\x304F" L"\x3060" L"\x3055" L"\x3044", //JP

    L"Auf dieser Festplatte ist nicht\n"
    L"gen" L"\xFC" L"gend Platz. Bitte %lu\n"
    L"Bl" L"\xF6" L"cke frei machen.",// DE

    L"Le disque dur ne comporte\n"
    L"pas assez de blocs libres.\n"
    L"Il faut effacer %lu blocs.", // FR

    L"El disco de memoria no\n"
    L"dispone de suficientes bloques\n"
    L"libres. Se necesitan %lu\n"
    L"bloques. Presione A para\n"
    L"continuar.", // ES

    L"Sul disco rigido non ci sono\n"
    L"blocchi liberi sufficienti per\n"
    L"questo salvataggio. Occorre\n"
    L"liberare %lu blocchi.", // IT
};

const WCHAR* const strNO_SAVES[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"No saved games",

    L"\x4FDD" L"\x5B58" L"\x3057" L"\x305F" L"\x30B2" L"\x30FC" L"\x30E0" 
    L"\x306F" L"\x3042" L"\x308A" L"\x307E" L"\x305B" L"\x3093",

    L"Kein Spielst" L"\xE4" L"nde vorhanden.",// DE

    L"Aucune partie sauvegard" L"\xE9" L"e", // FR

    L"No hay partidas guardadas", // ES

    L"Nessun salvataggio", // IT
};

// TCR 6-3 MU Read Warning
const WCHAR* const strLOADING[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"Loading Game.\n",

    L"\x30B2" L"\x30FC" L"\x30E0" L"\x3092" L"\x8AAD" L"\x307F" L"\x8FBC" 
    L"\x307F" L"\x4E2D" L"\x3067" L"\x3059" L".\n",

    L"Spiel wird geladen.\n", // DE

    L"Chargement de la\n"
    L"partie en cours.\n", // FR

    L"Cargando partida.\n", // ES

    L"Caricamento partita in corso.\n", // IT
};


// TCR 6-2 MU Write Warning
const WCHAR* const strSAVING_MU[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"Saving game to memory unit\n%s.\n",

    L"\x30E1" L"\x30E2" L"\x30EA" L" " L"\x30E6" L"\x30CB" L"\x30C3" L"\x30C8" 
	L"\x306B" L"\x30B2" L"\x30FC" L"\x30E0" L"\x3092" L"\x4FDD" L"\x5B58" L"\x4E2D" L"\n%s",

    L"Spiel wird auf\n"
    L"Speicher-Einheitgespeichert\n%s.\n", // DE

    L"Sauvegarde en cours sur\n"
    L"la Carte m" L"\x00E9" L"moire\n%s.\n", // FR

    L"Guardando partida en\n"
    L"unidad de memoria\n%s.\n", // ES

    L"Salvataggio su memory unit\n"
    L"in corso\n%s.\n", // IT
};

// TCR 6-4 Hard Disk Write Warning
const WCHAR* const strSAVING[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"Saving Game.\n",

    L"\x30B2" L"\x30FC" L"\x30E0" L"\x3092" L"\x4FDD" L"\x5B58" L"\x4E2D" 
    L"\x3067" L"\x3059" L".\n",

    L"Spiel wird gespeichert.\n", // DE

    L"Sauvegarde de la\n"
    L"partie en cours.\n", // FR

    L"Guardando partida.\n", // ES

    L"Salvataggio partita in corso.\n", // IT
};

// TCR 6-2 MU Write Warning
// TCR 6-3 MU Read Warning
const WCHAR* const strDO_NOT_REMOVE_MU[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"Please don't remove memory\n"
    L"units or disconnect any\n"
    L"controllers.",

    L"\x30E1" L"\x30E2" L"\x30EA" L"\x0020" L"\x30E6" L"\x30CB" 
    L"\x30C3" L"\x30C8" L"\x3084" L"\x30B3" L"\x30F3" L"\x30C8" L"\x30ED" L"\n"
    L"\x30FC" L"\x30E9" L"\x3092" L"\x53D6" L"\x308A" L"\x5916" L"\x3055" 
    L"\x306A" L"\x3044" L"\x3067" L"\x304F" L"\x3060" L"\x3055" L"\x3044" L"\x3002",

    L"Bitte entfernen Sie keine\n"
    L"Speicher-Einheiten und\n"
    L"keine Controller.",// DE

    L"Ne pas retirer de Carte\n"
    L"m" L"\xE9" L"moire ou d" L"\xE9" L"connecter\n"
    L"de manette.", // FR

    L"No retire las Unidades de\n"
    L"Memoria ni desconecte\n"
    L"los controladores.", // ES

    L"Non rimuovere alcun\n"
    L"controller o unit" L"\xE0" L" di memoria.", // IT
};

// TCR 6-4 Hard Disk Write Warning
const WCHAR* const strDO_NOT_POWEROFF[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"Please don't turn\n"
    L"off your Xbox console.\n",

    L"Xbox\n" L"\x306E" L"\x96FB" L"\x6E90" L"\x3092" 
    L"\x5207" L"\x3089" L"\x306A" L"\x3044" L"\x3067" L"\x304F" 
    L"\x3060" L"\x3055" L"\x3044" L"\x3002",  //JP

    L"Bitte schalten Sie\n"
    L"Ihre Xbox nicht aus.", // DE

    L"Ne pas " L"\xE9" L"teindre\n"
    L"la console Xbox.", // FR

    L"No apague la Xbox.", // ES

    L"Non spegnere Xbox.", // IT
};

const WCHAR* const strSAVE_GAME[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Save Game",
    L"\x30B2" L"\x30FC" L"\x30E0" L"\x306E" L"\x4FDD" L"\x5B58",
    L"Spiel speichern", // DE
    L"Sauvegarder la partie", // FR
    L"Guardar partida", // ES
    L"Salva partita", // IT
};

const WCHAR* const strLOAD_GAME[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Load Game",
    L"\x30B2" L"\x30FC" L"\x30E0" L"\x306E" L"\x8AAD" L"\x307F" L"\x8FBC" L"\x307F",
    L"Spiel laden", // DE
    L"Charger la partie", // FR
    L"Cargar partida", // ES
    L"Carica partita", // IT
};

const WCHAR* const strILLUS_GRAPHICS[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Sample graphics. Don't use in your game",

    L"\x30B5" L"\x30F3" L"\x30D7" L"\x30EB" L"\x0020" L"\x30B0" L"\x30E9" 
    L"\x30D5" L"\x30A3" L"\x30C3" L"\x30AF" L"\x3002" L"\n" L"\x4F5C" 
    L"\x6210" L"\x3059" L"\x308B" L"\x30B2" L"\x30FC" L"\x30E0" L"\x3067" 
    L"\x4F7F" L"\x7528" L"\x3057" L"\x306A" L"\x3044" L"\x3067" L"\x304F" 
    L"\x3060" L"\x3055" L"\x3044",
    
    L"Beispiel-Grafik. Nicht im Spiel verwenden.",// DE
    
    L"Exemples de graphismes.\n"
    L"Ne pas utiliser dans votre jeu.", // FR
    
    L"Gr" L"\xE1" L"ficos de muestra.\n"
    L"No utilizar en la partida.", // ES
    
    L"Immagine dimostrativa. Da non\n"
    L"utilizzare per i giochi da\n"
    L"voi sviluppati.", // IT
};

const WCHAR* const strFORMAT_DEVICE[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"Total blocks\n%hs\n"
    L"Available blocks\n%hs",

    L"\x7DCF" L"\x30D6" L"\x30ED" L"\x30C3" L"\x30AF" L"\x6570" L"\n%hs\n" 
    L"\x5229" L"\x7528" L"\x53EF" L"\x80FD" L"\x30D6" L"\x30ED" L"\x30C3" 
    L"\x30AF" L"\x6570" L"\n%hs",

    L"Gesamtzahl Bl" L"\xF6" L"cke\n%hs\n"
    L"Verf" L"\xFC" L"gbare Bl" L"\xF6" L"cke\n%hs", //DE

    L"Nombre de blocs\n%hs\n"
    L"Blocs disponibles\n%hs", // FR

    L"Bloques totales\n%hs\n"
    L"Bloques libres\n%hs", // ES

    L"Blocchi totali\n%hs\n"
    L"Blocchi liberi\n%hs", // IT
};

const WCHAR* const strCHOOSE_LOAD[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Choose memory device to load game",

    L"\x30B2" L"\x30FC" L"\x30E0" L"\x3092" L"\x8AAD" L"\x307F" 
    L"\x8FBC" L"\x3080" L"\x30E1" L"\x30E2" L"\x30EA" L"\x0020" 
    L"\x30C7" L"\x30D0" L"\x30A4" L"\x30B9" L"\x3092" L"\x9078" 
    L"\x629E" L"\x3057" L"\x3066" L"\x304F" L"\x3060" L"\x3055" L"\x3044",

    L"Speichermedium w" L"\xE4" L"hlen, um Spiel zu laden",// DE

    L"Choisir un emplacement pour charger la partie", // FR

    L"Elija un dispositivo de memoria para cargar la partida", // ES

    L"Scegliere la periferica di memoria da cui caricare il gioco", // IT
};

const WCHAR* const strCHOOSE_SAVE[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Choose memory device to save game",

    L"\x30B2" L"\x30FC" L"\x30E0" L"\x3092" L"\x4FDD" L"\x5B58" L"\x3059" 
    L"\x308B" L"\x30E1" L"\x30E2" L"\x30EA" L"\x0020" L"\x30C7" L"\x30D0" 
    L"\x30A4" L"\x30B9" L"\x3092" L"\x9078" L"\x629E" L"\x3057" L"\x3066" 
    L"\x304F" L"\x3060" L"\x3055" L"\x3044",

    L"Speichermedium w" L"\xE4" L"hlen, um Spiel zu speichern",// DE

    L"Choisir un emplacement pour sauvegarder la partie", // FR

    L"Elija un dispositivo de memoria para guardar la partida", // ES

    L"Scegliere su quale dispositivo di memoria si vuole salvare la partita", // IT
};

const WCHAR* const strTODAY[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Today",
    L"\x4ECA" L"\x65E5", 
    L"Heute", // DE
    L"Aujourd'hui", // FR
    L"Hoy", // ES
    L"Oggi", // IT
};

const WCHAR* const strNOW[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Now",
    L"L\x73FE" L"\x5728",
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

    L"\x540D" L"\x524D" L" : %s\n" L"\x30D6" L"\x30ED" L"\x30C3" 
    L"\x30AF" L" : %hs\n" L"\x65E5" L"\x4ED8"  L": %s\n" 
    L"\x6642" L"\x523B" L" : %s",

    L"Name: %s\nBl" L"\xF6" L"cke: %hs\n"
    L"Datum: %s\nZeit: %s",// DE

    L"Nom: %s\nBlocs: %hs\n"
    L"Date: %s\nHeure: %s", // FR

    L"Nombre: %s\nBloques: %hs\n"
    L"Fecha: %s\nHora: %s", // ES

    L"Nome: %s\nBlocchi: %hs\n"
    L"Data: %s\nOra: %s", // IT
};

const WCHAR* const strEMPTY_SPACE[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Empty Space",
    L"\x7A7A" L"\x304D" L"\x5BB9" L"\x91CF",
    L"Freier Platz", // DE
    L"M" L"\xE9" L"moire disponible", // FR
    L"Espacio vac" L"\xED" L"o", // ES
    L"Spazio libero", // IT
};

const WCHAR* const strA_SELECT[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"select",
    L"\x9078" L"\x629E",
    L"ausw" L"\xE4" L"hlen", // DE
    L"s" L"\xE9" L"lectionner", // FR               
    L"seleccionar", // ES
    L"scegli", // IT
};

const WCHAR* const strB_BACK[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"back",
    L"\x623B" L"\x308B",
    L"zur" L"\xFC" L"ck", // DE
    L"retour", // FR
    L"volver", // ES
    L"indietro", // IT
};

const WCHAR* const strY_DELETE[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"delete",
    L"\x524A" L"\x9664",
    L"l" L"\xF6" L"schen", // DE
    L"supprimer", // FR
    L"eliminar", // ES
    L"cancella", // IT
};

const WCHAR* const strOVERWRITE[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"Overwrite saved game?",

    L"\x4FDD" L"\x5B58" L"\x3057" L"\x305F" L"\x30B2" L"\x30FC" 
    L"\x30E0" L"\x3092" L"\x4E0A" L"\x66F8" L"\x304D" L"\x3057" 
    L"\x307E" L"\x3059" L"\x304B" L"\003F",

    L"Spielstand " L"\xFC" L"berschreiben?", //DE
    
    L"\xC9" L"craser la partie\n"
    L"sauvegard" L"\xE9" L"e ?", // FR

    L"\xBF" L"Desea sobrescribir\n"
    L"la partida guardada?", // ES

    L"Si vuole sovrascrivere\n"
    L"il salvataggio?", // IT
};

const WCHAR* const strYES[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Yes",
    L"\x306F" L"\x3044",
    L"Ja", // DE
    L"Oui", // FR
    L"S" L"\xED" L"", // ES
    L"S" L"\xEC" L"", // IT
};

const WCHAR* const strNO[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"No",
    L"\x3044" L"\x3044" L"\x3048",
    L"Nein", // DE
    L"Non", // FR
    L"No", // ES
    L"No", // IT
};

const WCHAR* const strDELETE[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"Delete saved game?",

    L"\x4FDD" L"\x5B58" L"\x3057" L"\x305F" L"\x30B2" L"\x30FC" 
    L"\x30E0" L"\x3092" L"\x524A" L"\x9664" L"\x3057" L"\x307E" 
    L"\x3059" L"\x304B" L"\003F",

    L"Spielstand l" L"\xF6" L"schen?", //DE
    
    L"Supprimer la partie\n"
    L"sauvegard" L"\xE9" L"e ?", // FR

    L"\xBF" L"Desea eliminar\n"
    L"la partida guardada?", // ES

    L"Si vuole eliminare\n"
    L"il salvataggio?", // IT
};

const WCHAR* const strLOADING_GAME_LIST[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"Loading game list",

    L"\x30B2" L"\x30FC" L"\x30E0" L"\x306E" L"\x4E00" L"\x89A7" L"\x3092" 
    L"\x8AAD" L"\x307F" L"\x8FBC" L"\x307F" L"\x4E2D" L"\x3067" L"\x3059",

    L"Spieleliste wird geladen", // DE

    L"Chargement de la liste\n"
    L"des parties", // FR

    L"Cargando lista\n"
    L"de partidas", // ES

    L"Caricamento elenco\n"
    L"partite in corso", // IT
};

const WCHAR* const strMAX_SAVED_GAMES[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"There is a maximum of\n"
    L"4,096 saved games.",

    L"\x3053" L"\x306E" L"\x30C7" L"\x30D0" L"\x30A4" L"\x30B9" 
    L"\x306B" L"\x306F" L" 4097 " L"\x4EF6" L"\x4EE5" L"\x4E0A" 
    L"\x306E" L"\x30B2" L"\x30FC" L"\x30E0" L"\x3092" L"\x4FDD" 
    L"\x5B58" L"\x3059" L"\x308B" L"\x3053" L"\x3068" L"\x306F" 
    L"\x3067" L"\x304D" L"\x307E" L"\x305B" L"\x3093" L"\x3002",  //JP  Needs to be updated.

    L"Speichern von mehr als 4.096\n"
    L"Spielen auf diesem Medium\n"
    L"nicht m" L"\xF6" L"glich.", //DE
    
    L"Impossible de sauvegarder\n"
    L"plus de 4 096 parties sur\n"
    L"le disque dur.", // FR

    L"No puede guardar m" L"\xE1" L"s\n"
    L"de 4.096 partidas en este\n"
    L"dispositivo.", // ES

    L"Questa periferica non pu" L"\xF2" L" \n"
    L"contenere pi" L"\xF9" L" di 4.096 salvataggi.", // IT
};

const WCHAR* const strMU_UNUSABLE[ XC_LANGUAGE_MAX ] = 
{
    L"",

    L"This memory unit is unusable",

    L"\x3053" L"\x306E" L"\x30E1" L"\x30E2" L"\x30EA" L"\x0020" L"\x30E6" 
    L"\x30CB" L"\x30C3" L"\x30C8" L"\x306F" L"\x52D5" L"\x4F5C" L"\x3057" 
    L"\x3066" L"\x3044" L"\x307E" L"\x305B" L"\x3093" L"\x3002" L"\n" L"\x30E1" 
    L"\x30E2" L"\x30EA" L"\x0020" L"\x30E6" L"\x30CB" L"\x30C3" L"\x30C8" 
    L"\x304C" L"\x7834" L"\x640D" L"\x3057" L"\x3066" L"\x3044" L"\x308B" 
    L"\x53EF" L"\x80FD" L"\x6027" L"\x304C" L"\x3042" L"\x308A" L"\x307E" 
    L"\x3059" L"\x3002",

    L"Diese Speicher-Einheit\n"
    L"funktioniert nicht. M" L"\xF6" L"glicher-\n"
    L"weise ist sie defekt.", //DE
    
    L"Cette Carte m" L"\xE9" L"moire ne\n"
    L"fonctionne pas. Elle est\n"
    L"peut-" L"\xEA" L"tre endommag" L"\xE9" L"e.", // FR

    L"La Unidad de Memoria no\n"
    L"funciona. Podr" L"\xED" L"a estar\n"
    L"da" L"\xF1" L"ada.", // ES

    L"Questa unit" L"\xE0" L" di memoria non\n"
    L"risponde: potrebbe essere\n"
    L"danneggiata.", // IT
};

const WCHAR* const strMU_FULL[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"This memory unit is full",

    L"\x30E1" L"\x30E2" L"\x30EA" L" " L"\x30E6" L"\x30CB" L"\x30C3" 
	L"\x30C8" L"\x304C" L"\x3044" L"\x3063" L"\x3071" L"\x3044" L"\x3067" L"\x3059" , // JP

    L"Diese Speicher-Einheit ist voll", // DE
    L"La Carte m" L"\x00E9"  L"moire est pleine", // FR
    L"Esta unidad de memoria est" L"\x00E1" L"llena", // ES
    L"Questa memory unit " L"\x00E8" L"piena", // IT
};

const WCHAR* const strUNUSABLE_MU_NAME[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Unusable Xbox MU",
    L"\x7834" L"\x640D" L"\x3057" L"\x305F" L" Xbox " L"\x30E1" 
    L"\x30E2" L"\x30EA" L"\x0020" L"\x30E6" L"\x30CB" L"\x30C3" L"\x30C8",
    L"Xbox-SE besch" L"\xE4" L"digt", // DE
    L"CM Xbox endommag" L"\xE9" L"e", // FR
    L"UM de la Xbox da" L"\xF1" L"ada", // ES
    L"UM Xbox danneggiata", // IT
};

const WCHAR* const strFULL_MU_NAME[ XC_LANGUAGE_MAX ] = 
{
    L"",
    L"Full Xbox MU",

    L"\x7A7A" L"\x304D" L"\x5BB9" L"\x91CF" L"\x304C" L"\x306A" L"\x3044" 
	L" Xbox " L"\x30E1" L"\x30E2" L"\x30EA" L" " L"\x30E6" L"\x30CB" L"\x30C3" L"\x30C8",

    L"Xbox-SE voll.", // DE
    L"CM Xbox pleine", // FR
    L"UM Xbox llena", // ES
    L"MU Xbox piena", // IT
};

// TCR 6-1 Surprise MU Removal!
const WCHAR* const strMU_REMOVED[ XC_LANGUAGE_MAX ] =
{
    L"",

    L"%s\nwas removed during use.\n"
    L"%s failed.\n"
    L"Press A to continue.",

    L"%s " L"\x304C" L"\x4F7F" L"\x7528" L"\x4E2D" L"\x306B" L"\x53D6" 
    L"\x308A" L"\x5916" L"\x3055" L"\x308C" L"\x307E" L"\x3057" L"\x305F" 
    L"\x3002" L"\n%s " L"\x3067" L"\x554F" L"\x984C" L"\x304C" L"\x767A" 
    L"\x751F" L"\x3057" L"\x307E" L"\x3057" L"\x305F" L"\x3002" L"\n A "  
    L"\x30DC" L"\x30BF" L"\x30F3" L"\x3092" L"\x62BC" L"\x3057" L"\x3066" 
    L"\x7D9A" L"\x3051" L"\x3066" L"\x304F" L"\x3060" L"\x3055" L"\x3044" L"\x3002",

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
    L"\x4FDD" L"\x5B58",
    L"Speichern", // DE
    L"Sauvegarder", // FR
    L"Guardar", // ES
    L"Salva", // IT
};

const WCHAR* const strACTION_LOAD[ XC_LANGUAGE_MAX ] =
{
    L"",
    L"Load",
    L"\x8AAD" L"\x307F" L"\x8FBC" L"\x307F",
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
