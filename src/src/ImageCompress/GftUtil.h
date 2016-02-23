
#ifndef __GFTUTILITY_H__
#define __GFTUTILITY_H__

#include <cstdint>
#include <vector>

namespace GftUtil
{
#pragma pack(push, 1)
    typedef struct tagGFTFILEHEADER {
        uint32_t i32Type;
        uint16_t i16Version;
        uint16_t i16Reserved;
        uint32_t i32Reserved;
        uint16_t i16TextureMode;
        uint16_t i16BlendMode;
        uint32_t i32DataOffset;
    } GFTFILEHEADER;


    typedef struct tagGFTNINEGRIDINFO {
        uint32_t i32Type;
        uint32_t i32Size;
        uint16_t i16Left;
        uint16_t i16Top;
        uint16_t i16Right;
        uint16_t i16Bottom;
    } GFTNINEGRIDINFO;
#pragma pack(pop)

    bool GetNinePathInfo(HANDLE file_handle, RECT &nine_path)
    {
        ::SetFilePointer(file_handle, 0, 0, FILE_BEGIN);
        DWORD uRead = 0;
        GFTFILEHEADER gftHeader = { 0 };
        ::ReadFile(file_handle, &gftHeader, sizeof(gftHeader), &uRead, NULL);
        if (uRead != sizeof(GFTFILEHEADER))
        {
            return false;
        }

        std::vector<byte> buffer;
        byte* pInfoHeader = nullptr;
        size_t uInfoSize = 0;
        size_t uTell = ::SetFilePointer(file_handle, 0, 0, FILE_CURRENT); // get
        if (gftHeader.i32DataOffset == 0)          // no pic data
        {
            // here goes uInfoSize == 0
        }
        else if (uTell > gftHeader.i32DataOffset)
        {
            return false;
        }
        else if (uTell < gftHeader.i32DataOffset)
        {
            uInfoSize = gftHeader.i32DataOffset - uTell;
            if (uInfoSize > 0x3FFF)    // too large
            {
                return false;
            }
            buffer.resize(uInfoSize);
            pInfoHeader = buffer.data();
            if (!pInfoHeader)
            {
                return false;
            }
            ::ReadFile(file_handle, pInfoHeader, uInfoSize, &uRead, NULL);
            if (uRead != uInfoSize)
            {
                return false;
            }
        }

        size_t uCurPos = 0;
        while (true)
        {
            if (uInfoSize < uCurPos + 8)
            {
                break;
            }
            if (pInfoHeader[uCurPos + 0] == (byte)'N'
                && pInfoHeader[uCurPos + 1] == (byte)'I'
                && pInfoHeader[uCurPos + 2] == (byte)'N'
                && pInfoHeader[uCurPos + 3] == (byte)'E')
            {
                GFTNINEGRIDINFO* pNineGrid = (GFTNINEGRIDINFO*)(pInfoHeader + uCurPos);
                if (pNineGrid->i32Size < 8)
                {
                    break;
                }
                if (pNineGrid->i32Size == sizeof(GFTNINEGRIDINFO) && uCurPos + pNineGrid->i32Size <= uInfoSize)
                {
                    nine_path.left = pNineGrid->i16Left;
                    nine_path.top = pNineGrid->i16Top;
                    nine_path.right = pNineGrid->i16Right;
                    nine_path.bottom = pNineGrid->i16Bottom;
                    return true;
                }
                uCurPos += pNineGrid->i32Size;
            }
            else
            {
                uint32_t * pSize = (uint32_t *)(pInfoHeader + uCurPos + 4);
                if (*pSize < 8)
                {
                    break;
                }
                uCurPos += *pSize;
            }
        }

        return false;
    }

    bool setNinePathInfo(HANDLE file_handle, RECT &nine_path)
    {
        ::SetFilePointer(file_handle, 0, 0, FILE_BEGIN);
        DWORD uRead = 0;
        GFTFILEHEADER gftHeader = { 0 };
        ::ReadFile(file_handle, &gftHeader, sizeof(gftHeader), &uRead, NULL);
        if (uRead != sizeof(GFTFILEHEADER))
        {
            return false;
        }

        std::vector<byte> buffer;
        byte* pInfoHeader = nullptr;
        size_t uInfoSize = 0;
        size_t uTell = ::SetFilePointer(file_handle, 0, 0, FILE_CURRENT); // get
        if (gftHeader.i32DataOffset == 0)          // no pic data
        {
            // here goes uInfoSize == 0
        }
        else if (uTell > gftHeader.i32DataOffset)
        {
            return false;
        }
        else if (uTell < gftHeader.i32DataOffset)
        {
            uInfoSize = gftHeader.i32DataOffset - uTell;
            if (uInfoSize > 0x3FFF)    // too large
            {
                return false;
            }
            buffer.resize(uInfoSize);
            pInfoHeader = buffer.data();
            if (!pInfoHeader)
            {
                return false;
            }
            ::ReadFile(file_handle, pInfoHeader, uInfoSize, &uRead, NULL);
            if (uRead != uInfoSize)
            {
                return false;
            }
        }

        size_t uCurPos = 0;
        while (true)
        {
            if (uInfoSize < uCurPos + 8)
            {
                break;
            }
            if (pInfoHeader[uCurPos + 0] == (byte)'N'
                && pInfoHeader[uCurPos + 1] == (byte)'I'
                && pInfoHeader[uCurPos + 2] == (byte)'N'
                && pInfoHeader[uCurPos + 3] == (byte)'E')
            {
                GFTNINEGRIDINFO* pNineGrid = (GFTNINEGRIDINFO*)(pInfoHeader + uCurPos);
                if (pNineGrid->i32Size < 8)
                {
                    break;
                }
                if (pNineGrid->i32Size == sizeof(GFTNINEGRIDINFO) && uCurPos + pNineGrid->i32Size <= uInfoSize)
                {
                    pNineGrid->i16Left = static_cast<uint16_t>(nine_path.left);
                    pNineGrid->i16Top = static_cast<uint16_t>(nine_path.top);
                    pNineGrid->i16Right = static_cast<uint16_t>(nine_path.right);
                    pNineGrid->i16Bottom = static_cast<uint16_t>(nine_path.bottom);
                    ::SetFilePointer(file_handle, uTell, 0, FILE_BEGIN);
                    ::WriteFile(file_handle, pInfoHeader, uInfoSize, &uRead, NULL);
                    return true;
                }
                uCurPos += pNineGrid->i32Size;
            }
            else
            {
                uint32_t * pSize = (uint32_t *)(pInfoHeader + uCurPos + 4);
                if (*pSize < 8)
                {
                    break;
                }
                uCurPos += *pSize;
            }
        }

        return false;
    }
}

#endif
