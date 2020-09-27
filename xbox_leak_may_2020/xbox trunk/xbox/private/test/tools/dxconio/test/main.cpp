
#include <xtl.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "dxconio.h"

#define countof(x)      (sizeof(x) / sizeof(*(x)))

static LPTSTR g_pszText[] = {

    {TEXT("The Masque of the Red Death")},
    {TEXT("")},
    {TEXT("by Edgar Allan Poe (1842)")},
    {TEXT("")},
    {TEXT("The 'RED DEATH' had long devastated the country. No pestilence had ever been ")},
    {TEXT("so fatal or so hideous. Blood was its Avatar and its seal--the redness and the ")},
    {TEXT("horror of blood. There were sharp pains, and sudden dizziness, and then profuse ")},
    {TEXT("bleeding at the pores, with dissolution. The scarlet stains upon the body, and ")},
    {TEXT("especially upon the face of the victim, were the pest ban which shut him out ")},
    {TEXT("from the aid and from the sympathy of his fellow-men; and the whole seizure, ")},
    {TEXT("progress, and termination of the disease, were the incidents of half-an-hour. ")},
    {TEXT("")},
    {TEXT("But Prince Prospero was happy and hauntless and sagacious. When his dominions ")},
    {TEXT("were half-depopulated, he summoned to his presence a thousand hale and light-")},
    {TEXT("hearted friends from among the knights and dames of his court, and with these ")},
    {TEXT("retired to the deep seclusion of one of his castellated abbeys. This was an ")},
    {TEXT("extensive and magnificent structure, the creation of the prince's own ")},
    {TEXT("eccentric yet august taste. A strange and lofty wall girdled it in. This wall ")},
    {TEXT("had gates of iron. The courtiers, having entered, brought furnaces and massy ")},
    {TEXT("hammers and welded the bolts. They resolved to leave means neither of ingress ")},
    {TEXT("or egress to the sudden impulses of despair from without or of frenzy from ")},
    {TEXT("within. The abbey was amply provisioned. With such precautions the courtiers ")},
    {TEXT("might bid defiance to contagion. The external world could take care of itself. ")},
    {TEXT("In the meantime it was folly to grieve or to think. The prince had provided ")},
    {TEXT("all the appliances of pleasure. There were buffoons, there were improvisatori, ")},
    {TEXT("there were ballet dancers, there were musicians, there was beauty, there was ")},
    {TEXT("wine. All these and security were within. Without was the 'Red Death.' ")},
    {TEXT("")},
    {TEXT("It was toward the close of the fifth or sixth month of his seclusion, and ")},
    {TEXT("while the pestilence raged most furiously abroad, that the Prince Prospero ")},
    {TEXT("entertained his thousand friends at a masked ball of the most unusual ")},
    {TEXT("magnificence. ")},
    {TEXT("")},
    {TEXT("It was a voluptuous scene that masquerade. But first let me tell of the rooms ")},
    {TEXT("in which it was held. There were seven--an imperial suite. In many places, ")},
    {TEXT("however, such suites form a long and straight vista, while the folding doors ")},
    {TEXT("slide back nearly to the walls on either hand, so that the view of the whole ")},
    {TEXT("extent is scarcely impeded. Here the case was very different, as might have ")},
    {TEXT("been expected from the duke's love of the bizarre. The apartments were so ")},
    {TEXT("irregularly disposed that the vision embraced but little more than one at a ")},
    {TEXT("time. There was a sharp turn at every twenty or thirty yards, and at each turn ")},
    {TEXT("a novel effect. To the right and left, in the middle of each wall, a tall and ")},
    {TEXT("narrow Gothic window looked out upon a closed corridor which pursued the ")},
    {TEXT("windings of the suite. These windows were of stained glass whose colour varied ")},
    {TEXT("in accordance with the prevailing hue of the decorations of the chamber into ")},
    {TEXT("which it opened. That at the eastern extremity was hung, for example, in blue, ")},
    {TEXT("and vividly blue were its windows. The second chamber was purple in its ")},
    {TEXT("ornaments and tapestries, and here the panes were purple. The third was green ")},
    {TEXT("throughout, and so were the casements. The fourth was furnished and lighted ")},
    {TEXT("with orange, the fifth with white, the sixth with violet. The seventh apartment ")},
    {TEXT("was closely shrouded in black velvet tapestries that hung all over the ceiling ")},
    {TEXT("and down the walls, falling in heavy folds upon a carpet of the same material ")},
    {TEXT("and hue. But in this chamber only the colour of the windows failed to ")},
    {TEXT("correspond with the decorations. The panes here were scarlet--a deep blood-")},
    {TEXT("colour. Now in no one of the seven apartments was there any lamp or candelabrum ")},
    {TEXT("amid the profusion of golden ornaments that lay scattered to and from or ")},
    {TEXT("depended from the roof. There was no light of any kind emanating from lamp or ")},
    {TEXT("candle within the suite of chambers; but in the corridors that followed the ")},
    {TEXT("suite there stood opposite to each window a heavy tripod bearing a brazier of ")},
    {TEXT("fire that projected its rays through the tinted glass and so glaringly ")},
    {TEXT("illumined the room. And thus were produced a multitude of gaudy and fantastic ")},
    {TEXT("appearances. But in the western or black chamber the effect of the firelight ")},
    {TEXT("that streamed upon the dark hangings, through the blood-tinted panes, was ")},
    {TEXT("ghastly in the extreme, and produced so wild a look upon the countenances of ")},
    {TEXT("those who entered that there were few of the company bold enough to set foot ")},
    {TEXT("within its precincts at all. ")},
    {TEXT("")},
    {TEXT("It was in this apartment also that there stood against the western wall a ")},
    {TEXT("gigantic clock of ebony. Its pendulum swung to and fro with a dull, heavy, ")},
    {TEXT("monotonous clang; and when the minute-hand made the circuit of the face, ")},
    {TEXT("and the hour was to be stricken, there came from the brazen lungs of the ")},
    {TEXT("clock a sound which was clear and loud, and deep, and exceedingly musical, ")},
    {TEXT("but of so peculiar a note and emphasis that, at each lapse of an hour, the ")},
    {TEXT("musicians of the orchestra were constrained to pause momentarily in their ")},
    {TEXT("performance to hearken to the sound; and thus the waltzers perforce ceased ")},
    {TEXT("their evolutions, and there was a brief disconcert of the whole gay company, ")},
    {TEXT("and while the chimes of the clock yet rang it was observed that the giddiest ")},
    {TEXT("grew pale, and the more aged and sedate passed their hands over their brows as ")},
    {TEXT("if in confused reverie or meditation; but when the echoes had fully ceased a ")},
    {TEXT("light laughter at once pervaded the assembly; the musicians looked at each ")},
    {TEXT("other and smiled as if at their own nervousness and folly and made whispering ")},
    {TEXT("vows each to the other that the next chiming of the clock should produce in ")},
    {TEXT("them no similar emotion, and then, after the lapse of sixty minutes (which ")},
    {TEXT("embrace three thousand and six hundred seconds of the time that flies), there ")},
    {TEXT("came yet another chiming of the clock, and then were the same disconcert and ")},
    {TEXT("tremulousness and meditation as before. ")},
    {TEXT("")},
    {TEXT("But in spite of these things it was a gay and magnificent revel. The tastes of ")},
    {TEXT("the duke were peculiar. He had a fine eye for colours and effects. He ")},
    {TEXT("disregarded the decora of mere fashion. His plans were bold and fiery, and his ")},
    {TEXT("conceptions glowed with barbaric luster. There are some who would have thought ")},
    {TEXT("him mad. His followers felt that he was not. It was necessary to hear, and see, ")},
    {TEXT("and touch him to be sure that he was not. ")},
    {TEXT("")},
    {TEXT("He had directed, in great part, the moveable embellishments of the seven ")},
    {TEXT("chambers, upon occasion of this great fete; and its was his own guiding taste ")},
    {TEXT("which had given character to the masqueraders. Be sure they were grotesque. ")},
    {TEXT("There were much glare and glitter and piquancy and phantasm--much of what has ")},
    {TEXT("been since see in 'Hernani.' There were arabesque figures with unsuited limbs ")},
    {TEXT("and appointments. There were delirious fancies such as the madman fashions. ")},
    {TEXT("There were much of the beautiful, much of the wanton, much of the bizarre, ")},
    {TEXT("something of the terrible, and not a little of that which might excited ")},
    {TEXT("disgust. To and from in the seven chambers there stalked, in fact, a multitude ")},
    {TEXT("of dreams. And these--the dreams--writhed in and about, taking hue from the ")},
    {TEXT("rooms, and causing the wild music of the orchestra to seem as the echo of their ")},
    {TEXT("steps. And, anon, there strikes the ebony clock which stands in the hall of the ")},
    {TEXT("velvet; and then, for a moment, all is still, and all is silent save the voice ")},
    {TEXT("of the clock. The dreams are stiff-frozen as they stand. But the echoes of the ")},
    {TEXT("chime die away--they have endured but an instant--and a light, half-subdued ")},
    {TEXT("laughter floats after them as they depart. And now again the music swells, and ")},
    {TEXT("the dreams live, and writhe to and fro more merrily than ever, taking hue from ")},
    {TEXT("the many tinted windows through which stream the rays from the tripods. But to ")},
    {TEXT("the chamber which lies most eastwardly of the seven, there are now none of the ")},
    {TEXT("maskers who venture; for the night is waning away; and there flows a ruddier ")},
    {TEXT("light through the blood-coloured panes; and the blackness of the sable drapery ")},
    {TEXT("appalls; and to him whose foot falls upon the sable carpet, there comes from ")},
    {TEXT("the near clock of ebony a muffled peal more solemnly emphatic than any which ")},
    {TEXT("reaches their ears who indulge in the more remote gaieties of the other ")},
    {TEXT("apartments. ")},
    {TEXT("")},
    {TEXT("But these other apartments were densely crowded, and in them beat feverishly ")},
    {TEXT("the heart of life. And the revel went whirlingly on, until at length there ")},
    {TEXT("commenced the sounding of midnight upon the clock. And then the music ceased, ")},
    {TEXT("as I have told; and the evolutions of the waltzers were quieted; and there was ")},
    {TEXT("an uneasy cessation of all things as before. But now there were twelve strokes ")},
    {TEXT("to be sounded by the bell of the clock; and thus it happened, perhaps that more ")},
    {TEXT("of thought crept, with more of time, into the meditations of the thoughtful ")},
    {TEXT("among those who reveled. And thus, too, it happened, perhaps, that before the ")},
    {TEXT("last echoes of the last chime had utterly sunk into silence, there were many ")},
    {TEXT("individuals in the crowd who had found leisure to became aware of the presence ")},
    {TEXT("of a masked figure which had arrested the attention of no single individual ")},
    {TEXT("before. And the rumour of this new presence having spread itself whisperingly ")},
    {TEXT("around, there arose at length from the whole company a buzz, or murmur, ")},
    {TEXT("expressive of disapprobation and surprise--then, finally, of terror, of horror, ")},
    {TEXT("and of disgust. ")},
    {TEXT("")},
    {TEXT("In an assembly of phantasms such as I have painted, it may well be supposed ")},
    {TEXT("that no ordinary appearance could have excited such sensation. In truth the ")},
    {TEXT("masquerade licence of the night was nearly unlimited; but the figure in ")},
    {TEXT("question had out-Heroded Herod, and gone beyond the bounds of even the most ")},
    {TEXT("reckless which cannot be touched without emotion. Even with the utterly lost, ")},
    {TEXT("to whom life and death are equally jests, there are matters of which no jest ")},
    {TEXT("can be made. The whole company indeed seemed now deeply to feel that in the ")},
    {TEXT("costume and bearing of the stranger neither wit nor propriety existed. The ")},
    {TEXT("figure was tall and gaunt, and shrouded from head to foot in the habiliments ")},
    {TEXT("of the grave. The mask which concealed the visage was made so nearly to ")},
    {TEXT("resemble the countenance of a stiffened corpse that the closest scrutiny must ")},
    {TEXT("have had difficulty in detecting the cheat. And yet all this might have been ")},
    {TEXT("endured, if not approved, by the mad revellers around. But the mummer had ")},
    {TEXT("gone so far as to assume the type of the Red Death. His vesture was dabbled ")},
    {TEXT("in blood--and his broad brow with all the features of the face, was ")},
    {TEXT("besprinkled with the scarlet horror. ")},
    {TEXT("")},
    {TEXT("When the eyes of Prince Prospero fell upon this spectral image (which with a ")},
    {TEXT("slow and solemn movement, as if more fully to sustain its rôle, stalked to and ")},
    {TEXT("from among the waltzers) he was seen to be convulsed in the first moment with ")},
    {TEXT("a strong shudder either of terror or distaste; but in the next his brow ")},
    {TEXT("reddened with rage. ")},
    {TEXT("")},
    {TEXT("'Who dares?' he demanded hoarsely of the courtiers who stood near him-- 'who ")},
    {TEXT("dares insult us with this blasphemous mockery? Seize him and unmask him, that ")},
    {TEXT("we may know whom we have to hang at sunrise from the battlements!' ")},
    {TEXT("")},
    {TEXT("It was in the eastern or blue chamber in which stood the Prince Prospero as he ")},
    {TEXT("uttered these words. They rang throughout the seven rooms loudly and clearly--")},
    {TEXT("for the prince was a bold and robust man, and the music had become hushed at ")},
    {TEXT("the waving of his hand. ")},
    {TEXT("")},
    {TEXT("It was in the blue room where stood the prince, with a group of pale courtiers ")},
    {TEXT("by his side. At first, as he spoke, there was a slight rushing movement of this ")},
    {TEXT("group in the direction of the intruder, who, at the moment was also near at ")},
    {TEXT("hand, and now, with deliberate and stately step, made closer approach to the ")},
    {TEXT("speaker. But, from a certain nameless awe with which the mad assumptions of the ")},
    {TEXT("mummer had inspired the whole party, there were found none who put forth hand ")},
    {TEXT("to seize him; so that unimpeded he passed within a yard of the prince's person; ")},
    {TEXT("and while the vast assembly, as if with one impulse, shrank from the centres ")},
    {TEXT("of the rooms to the walls, he made his way uninterruptedly, but with the same ")},
    {TEXT("solemn and measured step which had distinguished him from the first, through ")},
    {TEXT("the blue chamber to the purple--through the purple to the green--through the ")},
    {TEXT("green to the orange--through this again to the white--and even thence to the ")},
    {TEXT("velvet, ere a decided movement had been made to arrest him. It was then, ")},
    {TEXT("however, that Prince Prospero, maddening with rage and the shame of his own ")},
    {TEXT("momentary cowardice, rushed hurriedly through the sib chambers, while none ")},
    {TEXT("followed him on account of a deadly terror that had seized upon all. He bore ")},
    {TEXT("aloft a drawn dagger, and had approached in rapid impetuosity, to within three ")},
    {TEXT("or four feet of the retreating figure, when the latter, having attained the ")},
    {TEXT("extremity of the velvet apartment, turned suddenly and confronted his pursuer. ")},
    {TEXT("There was a sharp cry--and the dagger dropped gleaming upon the sable carpet, ")},
    {TEXT("upon which, instantly afterwards, fell prostrate in death the Prince Prospero. ")},
    {TEXT("Then summoning the wild courage of despair, a throng of the revellers at once ")},
    {TEXT("threw themselves into the black apartment, and, seizing the mummer, whose tall ")},
    {TEXT("figure stood erect and motionless within the shadows of the ebony clock, gasped ")},
    {TEXT("in unutterable horror at finding the grave cerements and corpse-like mask which ")},
    {TEXT("they handled with so violent a rudeness, untenanted by any tangible form. ")},
    {TEXT("")},
    {TEXT("And now was acknowledged the presence of the Red Death. He had come like a ")},
    {TEXT("thief in the night; and one by one dropped the revellers in the blood-bedewed ")},
    {TEXT("halls of their revel, and died each in the despairing posture of his fall; and ")},
    {TEXT("the life of the ebony clock went out with that of the last of the gay; and the ")},
    {TEXT("flames of the tripods expired; and darkness and decay and the Red Death held ")},
    {TEXT("illimitable dominion over all. ")},
};

//******************************************************************************
//
// Function:
//
//     main
//
// Description:
//
//     Entry point for the application.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
#ifndef XBOX
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR szCmdLine, int nCmdShow) 
#else
void __cdecl main()
#endif
{
    UINT i, len;

    xCreateConsole(NULL);

    // Don't use the default background texture
//    xSetBackgroundImage(NULL);

//    xSetTextColor(68, 255, 68, 255);
//    xSetBackgroundColor(255, 255, 255);
//    xSetDisplayWidth(80);
//    xSetOutputBufferSize(15600);
//    xSetBackgroundImage(TEXT("t:\\texture.bmp"));

    len = countof(g_pszText);

    while (TRUE) {
        for (i = 0; i < len; i++) {
            _tprintf(TEXT("%s\n"), g_pszText[i]);
        }
    }

    xReleaseConsole();

#ifndef XBOX
    return 0;
#endif
}
