#ifndef  __IMAGE_DECLARE__
#define  __IMAGE_DECLARE__

enum IMAGE_TYPE {
    IMAGE_TYPE_NONE   = 0,
    IMAGE_TYPE_BITMAP = 1,
};

class CImage
{
public:
    CImage() : m_fHaveBounding(FALSE) 
        {
            m_rectBounding.SetRectEmpty();
            m_sizeImage = CSize(0, 0);
        };
    virtual ~CImage() {};

public:
    virtual BOOL DrawImage(CDC* pcDC, int x, int y, BOOL bForeBackGround = TRUE) = 0;
    CSize        GetSize(void) const { return m_sizeImage; }
    void         SetBoundingRect(const CRect& rectBounding) 
                    { 
                      m_rectBounding = rectBounding; 
                      m_fHaveBounding = TRUE;
                    }
    void         ClearBoundingRect(void) 
                    { m_fHaveBounding = FALSE; }
protected:
    BOOL  m_fHaveBounding;
    CRect m_rectBounding;
    CSize m_sizeImage;
};

CImage* CreateImage(DWORD dwType, LPCTSTR);

#endif //__IMAGE_DECLARE__
