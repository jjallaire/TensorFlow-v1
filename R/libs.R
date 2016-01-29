
#' @useDynLib TensorFlow
#' @importFrom Rcpp evalCpp
NULL


# return locations of python and tensor flow shared libraries
sharedLibraryPaths <- function() {
   list(python = pythonLibraryPath(),
        tensorflow = tensorFlowLibraryPath())
}

# find the location of the python shared library
pythonLibraryPath <- function() {

   # determine config dir
   pythonConfigDir <- system("python-config --configdir", intern = TRUE)

   # look for libpython there
   pattern <- glob2rx(paste0("libpython*", .Platform$dynlib.ext))
   libpythonPath <- list.files(pythonConfigDir,
                               pattern = pattern,
                               full.names = TRUE)

   # if we found it then return it, otherwise return null
   if (length(libpythonPath) > 0)
      libpythonPath[[1]]
   else
      ""
}

# find the location of the tensor flow shared library
tensorFlowLibraryPath <- function() {

   # determine location of site-packages
   python <- Sys.which("python")
   sitePackages <- system("python -c 'import site; print \":\".join(site.getsitepackages())'",
                          intern = TRUE)
   sitePackages <- strsplit(sitePackages, ":", fixed = TRUE)[[1]]

   # look for tensor-flow directory
   tensorFlowLibDir <- NULL
   for (packageDir in sitePackages) {
      probeDir <- file.path(packageDir, "tensorflow", "python")
      if (dir.exists(probeDir)) {
         tensorFlowLibDir <- probeDir
         break
      }
   }

   # if we found it then look for the shared library
   if (!is.null(tensorFlowLibDir)) {
      tensorFlowLib <- file.path(tensorFlowLibDir,
                                 paste0("_pywrap_tensorflow", .Platform$dynlib.ext))
      if (file.exists(tensorFlowLib))
         return(tensorFlowLib)
   }

   # didn't find the shared library
   ""
}
