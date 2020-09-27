        case 0xc000:IRQLatch=V;IRQlow&=~1;
                    if(resetmode==1)
                     {IRQCount=IRQLatch;}
                    break;
        case 0xc001:resetmode=1;IRQlow&=~1;
                    IRQCount=IRQLatch;
                    break;
        case 0xE000:IRQa=0;IRQlow&=~1;
                    if(resetmode==1)
                     IRQCount=IRQLatch;
                    break;
        case 0xE001:IRQa=1;IRQlow&=~1;
                    if(resetmode==1)
                     IRQCount=IRQLatch;
                    break;

