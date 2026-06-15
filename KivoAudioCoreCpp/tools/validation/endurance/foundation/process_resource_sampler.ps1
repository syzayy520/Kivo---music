Set-StrictMode -Version Latest

if (-not ("KivoEndurance.NativeMethods" -as [type])) {
    Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;

namespace KivoEndurance {
    public static class NativeMethods {
        [DllImport("user32.dll")]
        public static extern uint GetGuiResources(
            IntPtr process,
            uint flags);
    }
}
"@
}

function Get-KivoProcessResourceSample {
    param(
        [Parameter(Mandatory = $true)]
        [System.Diagnostics.Process]$Process,
        [Parameter(Mandatory = $true)][DateTimeOffset]$Started
    )

    $Process.Refresh()
    return [PSCustomObject]@{
        captured_utc = [DateTimeOffset]::UtcNow.ToString("O")
        elapsed_seconds = [math]::Round(
            ([DateTimeOffset]::UtcNow - $Started).TotalSeconds,
            3)
        private_bytes = [int64]$Process.PrivateMemorySize64
        working_set_bytes = [int64]$Process.WorkingSet64
        handle_count = [int]$Process.HandleCount
        thread_count = [int]$Process.Threads.Count
        gdi_objects = [int][KivoEndurance.NativeMethods]::GetGuiResources(
            $Process.Handle,
            0)
        user_objects = [int][KivoEndurance.NativeMethods]::GetGuiResources(
            $Process.Handle,
            1)
        cpu_seconds = [math]::Round(
            $Process.TotalProcessorTime.TotalSeconds,
            3)
    }
}

function Get-KivoPositiveGrowth {
    param(
        [Parameter(Mandatory = $true)][int64]$Before,
        [Parameter(Mandatory = $true)][int64]$After
    )
    return [math]::Max(0, $After - $Before)
}

function Get-KivoResourceSummary {
    param(
        [Parameter(Mandatory = $true)][object[]]$Samples,
        [Parameter(Mandatory = $true)][int]$WarmupSeconds
    )

    if ($Samples.Count -eq 0) {
        return $null
    }
    $eligible = @(
        $Samples |
            Where-Object { $_.elapsed_seconds -ge $WarmupSeconds })
    $baseline = if ($eligible.Count -gt 0) {
        $eligible[0]
    } else {
        $Samples[0]
    }
    $final = $Samples[-1]
    return [PSCustomObject]@{
        sample_count = $Samples.Count
        baseline_elapsed_seconds = $baseline.elapsed_seconds
        final_elapsed_seconds = $final.elapsed_seconds
        baseline = $baseline
        final = $final
        peak = [ordered]@{
            private_bytes = ($Samples.private_bytes |
                Measure-Object -Maximum).Maximum
            working_set_bytes = ($Samples.working_set_bytes |
                Measure-Object -Maximum).Maximum
            handle_count = ($Samples.handle_count |
                Measure-Object -Maximum).Maximum
            thread_count = ($Samples.thread_count |
                Measure-Object -Maximum).Maximum
            gdi_objects = ($Samples.gdi_objects |
                Measure-Object -Maximum).Maximum
            user_objects = ($Samples.user_objects |
                Measure-Object -Maximum).Maximum
        }
        growth = [ordered]@{
            private_bytes = Get-KivoPositiveGrowth `
                -Before $baseline.private_bytes `
                -After $final.private_bytes
            working_set_bytes = Get-KivoPositiveGrowth `
                -Before $baseline.working_set_bytes `
                -After $final.working_set_bytes
            handle_count = Get-KivoPositiveGrowth `
                -Before $baseline.handle_count `
                -After $final.handle_count
            thread_count = Get-KivoPositiveGrowth `
                -Before $baseline.thread_count `
                -After $final.thread_count
            gdi_objects = Get-KivoPositiveGrowth `
                -Before $baseline.gdi_objects `
                -After $final.gdi_objects
            user_objects = Get-KivoPositiveGrowth `
                -Before $baseline.user_objects `
                -After $final.user_objects
        }
    }
}
