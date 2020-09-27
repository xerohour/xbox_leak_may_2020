
class CEditBox
{
    public:
        CEditBox(int iLeft, int iTop, int iWidth, int iHeight, CDraw &Draw);
        void ProcessInput(CDraw &Draw);
        void Draw(CDraw &Draw);
    protected:
        void NextLine();
        void PreviousLine();
    private:
        int m_iLeft;
        int m_iTop;
        int m_iWidth;
        int m_iHeight;
        int m_iLines;
        int m_iColumns;
        int m_iLineSpacing;
        int m_iCurrentLine;
        int m_iCurrentPos;
        WCHAR **m_LineBuffer;
};
