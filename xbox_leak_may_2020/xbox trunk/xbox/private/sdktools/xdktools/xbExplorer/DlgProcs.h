// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     DlgProcs.h
// Contents: Contains function and class declarations for the DlgProcs.cpp file
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  AboutDlgProc
// Purpose:   Message handler for About dialog. This function is automatically called by Windows
//            whenever a message needs to be sent to the dialog box (ie 'paint', 'move', etc).
// Arguments: hwnd          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK AboutDlgProc( HWND, UINT, WPARAM, LPARAM );

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ConfirmDeleteFolderDlg
// Purpose:   Message handler for the 'Confirm folder delete' dialog. This function is
//            automatically called by Windows whenever a message needs to be sent to the dialog box
//            (ie 'paint', 'move', etc).
// Arguments: hwnd          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK ConfirmDeleteFolderDlg( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ConfirmDeleteFileDlg
// Purpose:   Message handler for the 'Confirm file delete' dialog. This function is automatically
//            called by Windows whenever a message needs to be sent to the dialog box (ie 'paint',
//            'move', etc).
// Arguments: hwnd          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK ConfirmDeleteFileDlg( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ConfirmFileOverwriteDlg
// Purpose:   Message handler for the 'Confirm file overwrite' dialog. This function is automatically
//            called by Windows whenever a message needs to be sent to the dialog box (ie 'paint',
//            'move', etc).
// Arguments: hwnd          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK ConfirmFileOverwriteDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ConfirmFolderOverwriteDlg
// Purpose:   Message handler for the 'Confirm folder overwrite' dialog. This function is automatically
//            called by Windows whenever a message needs to be sent to the dialog box (ie 'paint',
//            'move', etc).
// Arguments: hwnd          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK ConfirmFolderOverwriteDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ConfirmMoveROFileDlg
// Purpose:   Message handler for the 'Confirm file move' dialog. This function is automatically
//            called by Windows whenever a message needs to be sent to the dialog box (ie 'paint',
//            'move', etc).
// Arguments: hwnd          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK ConfirmMoveROFileDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ConfirmMoveROFolderDlg
// Purpose:   Message handler for the 'Confirm folder move' dialog. This function is automatically
//            called by Windows whenever a message needs to be sent to the dialog box (ie 'paint',
//            'move', etc).
// Arguments: hwnd          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK ConfirmMoveROFolderDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
