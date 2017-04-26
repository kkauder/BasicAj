#! /bin/env csh
#foreach f ( AjResults/Tow*_Eff*[01]_R0.2_For*Geant.root ) 
#foreach f ( AjResults/Tow*_Eff*[01]_For*Geant.root )

# foreach f ( AjResults/ForRes*Geant.root AjResults/R0.2_ForRes*Geant.root AjResults/Tow*_Eff*_For*Geant.root ) 
#     echo
#     echo root -l -b -q ForResAj_MatchGeantToPythia.cxx+\(\"$f\"\)
#     root -l -b -q ForResAj_MatchGeantToPythia.cxx+\(\"$f\"\)
# end

foreach f ( AjResults/ForRes*MatchedTo*GeantMc.root AjResults/R0.2_ForRes*MatchedTo*GeantMc.root AjResults/Tow*_Eff*_ForRes*MatchedTo*GeantMc.root )
    # echo $f
    echo root -l -b -q ResAjPlotter.cxx\(\"$f\"\)
    root -l -b -q ResAjPlotter.cxx\(\"$f\"\)
end

