unit AWUSB;

interface

uses
  AwusbApi, AwusbAbout, Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, Menus;

const WM_ABOUT = WM_USER + 2000;

type
  TfrmMain = class(TForm)
  { Define all the objects in this application.
    Wish we could leave out the labels for clarity. }

    Label1:  TLabel;
    Label2:  TLabel;
    Label3:  TLabel;
    Label4:  TLabel;
    Label5:  TLabel;
    Label6:  TLabel;
    Label7:  TLabel;
    Label8:  TLabel;
    Label9:  TLabel;
    Label10: TLabel;
    Label11: TLabel;
    Label12: TLabel;
    Label13: TLabel;
    Label14: TLabel;
    Label15: TLabel;
    Label16: TLabel;
    Label17: TLabel;
    Label18: TLabel;
    Label19: TLabel;
    Label20: TLabel;
    Label21: TLabel;
    Label22: TLabel;
    Label23: TLabel;
    Label24: TLabel;
    Label25: TLabel;
    Label26: TLabel;
    Label27: TLabel;
    Label28: TLabel;
    Label29: TLabel;
    Label30: TLabel;
    Label31: TLabel;
    Label32: TLabel;
    Label33: TLabel;
    Label34: TLabel;
    Label35: TLabel;
    Label36: TLabel;
    Label37: TLabel;
    Label38: TLabel;
    Label39: TLabel;
    Label40: TLabel;
    Label41: TLabel;
    Label42: TLabel;
    Label43: TLabel;
    Label44: TLabel;
    Label45: TLabel;
    Label46: TLabel;
    Label47: TLabel;
    Label48: TLabel;
    Label49: TLabel;
    Label50: TLabel;
    Label51: TLabel;
    Label52: TLabel;
    Label53: TLabel;
    Label54: TLabel;
    Label55: TLabel;
    Label56: TLabel;
    Label57: TLabel;
    Label58: TLabel;
    Label59: TLabel;
    Label60: TLabel;

    txtDevNum: TEdit;

    btnOpen: TButton;
    btnClose: TButton;

    btnDir15: TButton;
    btnVal15: TButton;
    btnDir14: TButton;
    btnVal14: TButton;
    btnDir13: TButton;
    btnVal13: TButton;
    btnDir12: TButton;
    btnVal12: TButton;
    btnDir11: TButton;
    btnVal11: TButton;
    btnDir10: TButton;
    btnVal10: TButton;
    btnDir9:  TButton;
    btnVal9:  TButton;
    btnDir8:  TButton;
    btnVal8:  TButton;
    btnDir7:  TButton;
    btnVal7:  TButton;
    btnDir6:  TButton;
    btnVal6:  TButton;
    btnDir5:  TButton;
    btnVal5:  TButton;
    btnDir4:  TButton;
    btnVal4:  TButton;
    btnDir3:  TButton;
    btnVal3:  TButton;
    btnDir2:  TButton;
    btnVal2:  TButton;
    btnDir1:  TButton;
    btnVal1:  TButton;
    btnDir0:  TButton;
    btnVal0:  TButton;

    txtDebug: TEdit;

    procedure FormCreate(Sender: TObject);
    procedure OnOpen(Sender: TObject);
    procedure OnClose(Sender: TObject);
    procedure UpdateGUI();
    procedure DoDir(Sender: TObject);
    procedure DoVal(Sender: TObject);
    procedure AboutMessage(var Msg: TMsg; var Handled: Boolean);

  private
    { Private declarations }


  public
    { Public declarations }

  end;

{ create a place holder for the buttons, so they can be indexed and
  identified in a for/next loop }
type
        TButtonArray = array[0..15] of TButton;

{ global variables }
var
  frmMain: TfrmMain;
  Dir: WORD;
  Data: WORD;
  TDirArray: TButtonArray;
  TValArray: TButtonArray;

implementation

{$R *.DFM}

procedure TfrmMain.FormCreate(Sender: TObject);
var
    SysMenuHandle: HMENU;
begin
    { set up a message handling procedure so we can trap system messages }
    Application.OnMessage:= AboutMessage;
    { get temporary handle to the system menu, which is the menu
      that appears when the user right-clicks on the form's title bar }
    SysMenuHandle:= GetSystemMenu(Handle, False);
    { make sure the handle is valid }
    if (SysMenuHandle <> NULL) then
    begin
    { add a separator to the system menu, the last two values are ignored }
        AppendMenu(SysMenuHandle, MF_SEPARATOR, 0, '');
    { add an 'About...' menu item to the system menu, with our custom
      command ID = WM_ABOUT which was defined above }
        AppendMenu(SysMenuHandle, MF_STRING, WM_ABOUT, 'About AwusbIO...');
    end;
end;

procedure TfrmMain.AboutMessage(var Msg: TMsg; var Handled: Boolean);
begin
{ Check for the WM_SYSCOMMAND message, which is called when the user righ-clicks
  on the title bar. wParam in this case is the command ID for the menu item that was chosen. }
    if (Msg.message = WM_SYSCOMMAND) And (Msg.wParam = (WM_ABOUT And $FFF0)) then
    begin
    { safely create an instance of the about form }
    with TfrmAbout.Create(Self) do
        try
            { show as a dialog which requires a user response }
            ShowModal;
        finally
            { if it doesn't work (e.g., not enough memory), clean up}
            Free;
        end;
        { let the system know we handled this message}
        Handled:= True;
    end;
end;

procedure TfrmMain.OnOpen(Sender: TObject);
var
    devnum: DWORD;
    err: DWORD;

begin
    { reset the global variables }
    Dir:= 0;
    Data:= 0;

    { There has got to be a better way to do this! }
    { point the Dir button array at each individual Dir button }
    { this way we can call each (indexed) button in a for/next loop }
    TDirArray[0]:= btnDir0;
    TDirArray[1]:= btnDir1;
    TDirArray[2]:= btnDir2;
    TDirArray[3]:= btnDir3;
    TDirArray[4]:= btnDir4;
    TDirArray[5]:= btnDir5;
    TDirArray[6]:= btnDir6;
    TDirArray[7]:= btnDir7;
    TDirArray[8]:= btnDir8;
    TDirArray[9]:= btnDir9;
    TDirArray[10]:= btnDir10;
    TDirArray[11]:= btnDir11;
    TDirArray[12]:= btnDir12;
    TDirArray[13]:= btnDir13;
    TDirArray[14]:= btnDir14;
    TDirArray[15]:= btnDir15;

    { point the Val button array at each individual Val button }
    TValArray[0]:= btnVal0;
    TValArray[1]:= btnVal1;
    TValArray[2]:= btnVal2;
    TValArray[3]:= btnVal3;
    TValArray[4]:= btnVal4;
    TValArray[5]:= btnVal5;
    TValArray[6]:= btnVal6;
    TValArray[7]:= btnVal7;
    TValArray[8]:= btnVal8;
    TValArray[9]:= btnVal9;
    TValArray[10]:= btnVal10;
    TValArray[11]:= btnVal11;
    TValArray[12]:= btnVal12;
    TValArray[13]:= btnVal13;
    TValArray[14]:= btnVal14;
    TValArray[15]:= btnVal15;

    { Open USB device }
    devnum:= StrtoInt(txtDevNum.Text);
    err:= AwusbOpen(devnum);
    if (err <> AWUSB_OK) { if error }
    then txtDebug.Text:= 'Error: ' + AwusbErrorMessage(err)
    else txtDebug.Text:= 'Device #' + InttoStr(devnum) + ' Opened Successfully';

    { Set the board to match with the current GUI }
    err:= AwusbEnablePort(@Dir, 2);
    if (err <> AWUSB_OK) then txtDebug.Text:= 'Error: ' + AwusbErrorMessage(err);

    err:= AwusbOutPort(@Data, 2);
    if (err <> AWUSB_OK) then txtDebug.Text:= 'Error: ' + AwusbErrorMessage(err);

    UpdateGUI();
end;

procedure TfrmMain.OnClose(Sender: TObject);
var
    err: DWORD;
begin
    { Close device }
    err:= AwusbClose();
    if (err <> AWUSB_OK) then txtDebug.Text:= 'Error: ' + AwusbErrorMessage(err)
    else txtDebug.Text:= 'Device Closed';
end;

procedure TfrmMain.UpdateGUI();
var
        err: DWORD;
        i: Integer;
        Mask: WORD;
begin

        { get current I/O port state }
        err:= AwusbInPort(@Data, 2);
        if (err <> AWUSB_OK) then txtDebug.Text:= 'Error: ' + AwusbErrorMessage(err);

        { update GUI }
        for i:=0 to 15 do { change button caption according to the state }
        begin
                Mask:=1 shl i; { shl == bitwise left shift }
                if ((Dir and Mask)>0) then TDirArray[i].Caption:= 'Out'
                else TDirArray[i].Caption:= 'In';

                if((Data and Mask)>0) then TValArray[i].Caption:= 'High'
                else TValArray[i].Caption:= 'Low';
        end;
end;

procedure TfrmMain.DoDir(Sender:TObject);
{ This will be called when the user clicks on one of the Direction buttons.
  Each Direction button's "OnClick" event is pointed to this procedure.
  The I/O number is identified by determining which button was pressed. }
var
    i: Integer;
    Mask: WORD;
    err: DWORD;
begin
    for i:= 0 to 15 do
        begin
            if Sender = TDirArray[i] then { the button was # i }
                begin
                    Mask:= 1 shl i; { shl == bitwise left shift }
                    { invert the i'th bit of the Dir register }
                    if ((Dir and Mask)>0) then Dir:= (Dir and (not Mask))
                    else Dir:= (Dir or Mask);

                    { Enable port }
                    err:= AwusbEnablePort(@Dir,2);
                    if (err <> AWUSB_OK) then txtDebug.Text:= 'Error: ' + AwusbErrorMessage(err);

                    UpdateGUI();
                end;
        end;
end;

procedure TfrmMain.DoVal(Sender: TObject);
{ This will be called when the user clicks on one of the Value buttons.
  Each Value button's "OnClick" event is pointed to this procedure.
  The I/O number is identified by determining which button was pressed. }
var
    i: Integer;
    Mask: WORD;
    err: DWORD;
begin
    for i:= 0 to 15 do
        begin
            if Sender = TValArray[i] then { the button was # i }
                begin
                    Mask:= 1 shl i; { shl == bitwise left shift }
                    { invert the i'th bit of the Data register }
                    if ((Data and Mask)>0) then Data:= (Data and (not Mask))
                    else Data:= (Data or Mask);

                    { Output port }
                    err:=AwusbOutPort(@Data,2);
                    if (err <> AWUSB_OK) then txtDebug.Text:= 'Error: ' + AwusbErrorMessage(err);

                    UpdateGUI();
                end;
        end;
end;

end.
