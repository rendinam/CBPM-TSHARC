function [CorrectedButtons,gainApplied] = ApplyGainCorrectionToButtons(BPMName,Buttons)
% Adjust button values, based on Gain corrections, taking from  BPM_INST_Params.cfg, on Feb 20 2012
% used by ProcssProcData.m

    applyGain = 1;
    switch upper(BPMName)
        case ('10W')
            BPM_F_Blk_A_Gains = [5.09469 5.18798 4.61286 5.10182];
        case ('11W')
            BPM_F_Blk_A_Gains = [5.47234 4.94997 4.72906 4.84717];
        case ('12W')
            BPM_F_Blk_A_Gains = [5.42364 4.98595 4.86154 4.73001];
        case ('13W')
            BPM_F_Blk_A_Gains = [5.20604 4.86712 5.02107 4.90257];
        case ('14W')
            BPM_F_Blk_A_Gains = [5.16799 5.25211 4.84083 4.73474];
        otherwise
            applyGain = 0;
    end
    
    if (applyGain)
        [CorrectedButtons,gainApplied] = ApplyIt(BPM_F_Blk_A_Gains,Buttons);
    else
        CorrectedButtons = Buttons;
        gainApplied = 0;
    end

end

function [CorrectedButtons,gainApplied] = ApplyIt(BPM_Gains,Buttons)
            BPM_GainsNormalized = BPM_Gains/sum(BPM_Gains) * 4;
            sz = size(Buttons);
            CorrectedButtons = Buttons ./ (ones(sz(1),1) * BPM_GainsNormalized);
            gainApplied = 1;
end
