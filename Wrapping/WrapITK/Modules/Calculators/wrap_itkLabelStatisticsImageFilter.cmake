WRAP_CLASS("itk::LabelStatisticsImageFilter" POINTER)
  FOREACH(d ${WRAP_ITK_DIMS})
    FOREACH(t1 ${WRAP_ITK_SCALAR})
      FOREACH(t2 ${WRAP_ITK_USIGN_INT} ${WRAP_ITK_SIGN_INT})
        WRAP_TEMPLATE("${ITKM_I${t1}${d}}${ITKM_I${t2}${d}}"  "${ITKT_I${t1}${d}},${ITKT_I${t2}${d}}")
      ENDFOREACH(t2)
    ENDFOREACH(t1)
  ENDFOREACH(d)
END_WRAP_CLASS()
