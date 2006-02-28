// (c) 2005-2006 Marc-Antoine Ruel
//
// "Magical" code that helps to easily remap any key
#pragma once
#include "Term.h"


// Those key are grabbed a key up only.
inline bool IsDeadKey(int VKey)
{
    switch (VKey)
    {
        case VK_SNAPSHOT:   // This one because only WM_KEYUP is sent, not WM_KEYDOWN... :( because of snapshoting
        case VK_RWIN:    case VK_LWIN:
        case VK_CONTROL: case VK_LCONTROL: case VK_RCONTROL:
        case VK_MENU:    case VK_LMENU:    case VK_RMENU:
        case VK_SHIFT:   case VK_LSHIFT:   case VK_RSHIFT:
            return true;
        default:
            return false;
    }
}

// Differentiate between left/right and numpad Virtual Keys.
inline int FixVKey(int VKey, LPARAM Ctx)
{
    const BYTE ScanCode = ((Ctx>>16)&0xFF);
    const Ext = (Ctx&(1<<24));
    switch (VKey)
    {
    case VK_CONTROL: // ScanCode 1D
        return Ext ? VK_RCONTROL : VK_LCONTROL;
    case VK_MENU:    // ScanCode 38
        return Ext ? VK_RMENU : VK_LMENU;
    case VK_SHIFT: // Regarde le scan code
        if (ScanCode == 0x2A) return VK_LSHIFT;
        if (ScanCode == 0x36) return VK_RSHIFT;
        return VKey;

        // Converti en VK_NUMPAD_XXX si nécessaire
    case VK_INSERT:   case VK_DELETE:
    case VK_PRIOR:    case VK_NEXT:    case VK_END:   case VK_HOME:
    case VK_LEFT:     case VK_UP:      case VK_RIGHT: case VK_DOWN:
        if (!Ext) return 0x100|VKey;
        return VKey;
        // C'est l'inverse pour lui.
    case VK_RETURN:
        if (Ext) return 0x100|VKey;
        return VKey;

        // Ignore the extended bit. Same as default:
    case VK_LWIN:     case VK_RWIN:    case VK_APPS:  case VK_SNAPSHOT:
    case VK_NUMLOCK:
    case VK_MULTIPLY: case VK_ADD:     case VK_SEPARATOR:
    case VK_SUBTRACT: case VK_DECIMAL: case VK_DIVIDE:
        return VKey;

    default:
        ASSERT( !(Ctx&(1<<24)) );
        // INS, DEL, HOME, END, PAGE UP, PAGE DOWN, and arrow keys in the clusters to the left of the numeric keypad; and the divide (/) and ENTER keys in the numeric keypad. 
        return VKey;
    }
}

template<typename T>
struct similar : public std::binary_function<T, T, int>
{
    static const int max = 0xFF;
    int operator()(const T & A, const T & B) const
    {
        return A.similar(B);
    }
};
struct similar_vkey : public std::binary_function<int, int, int>
{
    static const int max = 0xFF;
    int operator()(const int A, const int B) const
    {
        if (A==B)
            return 0;
        for (size_t i = 0; i < k_SimilarKeys_size; ++i)
        {
            if ((k_SimilarKeys[i].key1 == A && k_SimilarKeys[i].key2 == B)
                || (k_SimilarKeys[i].key1 == B && k_SimilarKeys[i].key2 == A))
                return 1;
        }
        return max;
    }
};

// Returns 1 if equals or more if it is only similar
struct similar_modifiers : public std::binary_function<int, int, int>
{
    static const int max = 0xFF;
    int operator()(const int A, const int B) const
    {
        if (A==B)
            return 0;
        // Left(01) == Both(11); Right(10) == Both(11); Left(01) != Right(10)
        int i = 0;
        const int OrFlags = A|B;
        const int AndFlags = A&B;
        if ((OrFlags&M_Shift) && !(AndFlags&M_Shift))
            return max;
        if ((A&M_Shift) != (B&M_Shift))
            ++i;
        if ((OrFlags&M_Ctrl)  && !(AndFlags&M_Ctrl))
            return max;
        if ((A&M_Ctrl) != (B&M_Ctrl))
            ++i;
        if ((OrFlags&M_Alt)   && !(AndFlags&M_Alt))
            return max;
        if ((A&M_Alt) != (B&M_Alt))
            ++i;
        return i;
    }
};

struct Keystroke2MsgEx
{
    int Modifiers;
    int VirtualKeyCode;

    Keystroke2MsgEx(const keystroke_to_msg & msg)
        : Modifiers(msg.Modifiers)
        , VirtualKeyCode(msg.win32_key)
    {
        //FUNC_ENTER2("(%d, %X)", VirtualKeyCode, Modifiers);
    }

    Keystroke2MsgEx(int in_VirtualKeyCode)
        : Modifiers(0)
        , VirtualKeyCode(in_VirtualKeyCode)
    {
        if (GetKeyState(VK_LSHIFT)&0x800)
            Modifiers |= M_LShift;
        else if (GetKeyState(VK_RSHIFT)&0x800)
            Modifiers |= M_RShift;
        else if (GetKeyState(VK_SHIFT)&0x800)
            Modifiers |= M_Shift;

        if (GetKeyState(VK_LCONTROL)&0x800)
            Modifiers |= M_LCtrl;
        else if (GetKeyState(VK_RCONTROL)&0x800)
            Modifiers |= M_RCtrl;
        else if (GetKeyState(VK_CONTROL)&0x800)
            Modifiers |= M_Ctrl;

        if (GetKeyState(VK_LMENU)&0x800)
            Modifiers |= M_LAlt;
        else if (GetKeyState(VK_RMENU)&0x800)
            Modifiers |= M_RAlt;
        else if (GetKeyState(VK_MENU)&0x800)
            Modifiers |= M_Alt;

        FUNC_ENTER2("(%d) %X", VirtualKeyCode, Modifiers);
    }

    Keystroke2MsgEx(int in_VirtualKeyCode, int in_Modifiers)
        : Modifiers(in_Modifiers)
        , VirtualKeyCode(in_VirtualKeyCode)
    {
        FUNC_ENTER2("(%d, %X)", VirtualKeyCode, Modifiers);
    }

    bool operator < (const Keystroke2MsgEx & rhs) const
    {
        if (VirtualKeyCode!=rhs.VirtualKeyCode)
            return VirtualKeyCode<rhs.VirtualKeyCode;
        // None, Left, Right then Both
        if (Modifiers==rhs.Modifiers)
            return false;

        if ((Modifiers&M_Shift) != (rhs.Modifiers&M_Shift))
            return (Modifiers&M_Shift) < (rhs.Modifiers&M_Shift);

        if ((Modifiers&M_Ctrl) != (rhs.Modifiers&M_Ctrl))
            return (Modifiers&M_Ctrl) < (rhs.Modifiers&M_Ctrl);

        if ((Modifiers&M_Alt) != (rhs.Modifiers&M_Alt))
            return (Modifiers&M_Alt) < (rhs.Modifiers&M_Alt);
        
        // Problem here. some high bits of Modifiers are set.
        Break();
        return false;
    }
    bool operator == (const Keystroke2MsgEx & rhs) const {
        return (VirtualKeyCode == rhs.VirtualKeyCode)
            && (Modifiers == rhs.Modifiers);
    }
    int similar(const Keystroke2MsgEx & rhs) const {
        return similar_vkey()(VirtualKeyCode, rhs.VirtualKeyCode) + similar_modifiers()(Modifiers, rhs.Modifiers);
    }
};
