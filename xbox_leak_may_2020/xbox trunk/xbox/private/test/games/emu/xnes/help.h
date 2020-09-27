#define NHELP 12

char *helplist[NHELP]=
        {"F1      Help Screen",
         "F5/F7   Save/Load state save",
         "F9      Save Snapshot",
         "F10     Reset",
         "F12/ESC Exit",

         "C       Insert Coin",
         "V       View DIP switches",
         "D       Modify DIP switches",

         "1-8     Toggle DIP switches",
         "T/H     Select tint/hue",
         "+/-     Inc/Dec tint or hue",

         "0-9     Select state save"

        };

int helpoffs[NHELP]={24,0,0,0,0,16,0,0,16,0,0,16};


void DisplayHelp(void)
{
 char *XBoof;
 int x,y;
 XBoof=XBuf;

 XBoof+=272*12+24;
 for(y=0;y<192;y++)
  for(x=0;x<224;x++)
   XBoof[x+y*272]=0x80;

 DrawTextTrans(XBoof+56+544,272,"FCE Ultra Help",0x83);

 XBoof+=1;

 for(x=0;x<NHELP;x++)
  {
   XBoof+=helpoffs[x]*272;
   XBoof+=272<<3;
   DrawTextTrans(XBoof,272,helplist[x],0x83);
  }
}

