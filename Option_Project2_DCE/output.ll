; ModuleID = 'example.ll'
source_filename = "example.cpp"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @_Z3foov() #0 {
bb:
  br label %bb2

bb2:                                              ; preds = %bb13, %bb
  %tmp1.0 = phi i32 [ 0, %bb ], [ %tmp8, %bb13 ]
  %tmp.0 = phi i32 [ 0, %bb ], [ %tmp15, %bb13 ]
  %tmp4 = icmp slt i32 %tmp.0, 100
  br i1 %tmp4, label %bb5, label %bb2.bb16_crit_edge

bb2.bb16_crit_edge:                               ; preds = %bb2
  br label %bb16

bb5:                                              ; preds = %bb2
  %vSSA_sigma = phi i32 [ %tmp.0, %bb2 ]
  %tmp8 = add nsw i32 %tmp1.0, %vSSA_sigma
  br label %bb12

bb12:                                             ; preds = %bb5
  %vSSA_sigma1 = phi i32 [ %vSSA_sigma, %bb5 ]
  br label %bb13

bb13:                                             ; preds = %bb12
  %tmp15 = add nsw i32 %vSSA_sigma1, 1
  br label %bb2

bb16:                                             ; preds = %bb2.bb16_crit_edge
  ret i32 %tmp1.0
}

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 8.0.1 "}
