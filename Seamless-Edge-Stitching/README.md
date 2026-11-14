# Project Title: Implementing seamless image stitching using a weighted fast marching technique.
**Project Owner:** Mia Good \
**Whitepaper:** https://ietresearch.onlinelibrary.wiley.com/doi/epdf/10.1049/ipr2.12925 

## An overall rough flow of the program is as follows:

- Detect features in 2 images using SIFT
- Estimate a homography with RANSAC
- Warp the images onto the same plane
- Find the boundaries of each warped image
- Find the cross points of the images, these are the seam's start and end
- Using the image boundaries, the region where the images overlap is extracted from both images
- The feature maps (color, edge, texture, saliency) are computed using the cropped pair of images. 
	- Color uses the Euclidean RGB distance between the two images at each pixel
	- Edge merges the left and right edge maps per pixel by taking the maximum.
	- Texture takes the per-pixel difference between the left and right texture components
	- Saliency merges the left and right saliency maps per pixel by taking the maximum.
- Combine the 4 feature maps into a weighted cost map using weight coefficients
- Build the potential field from the starting seam point with FMM, use the weight map as the cost
- Extract the full minimum-cost seamline from the field (gradient descent)
- Perform a seamline path optimization: evaluate, truncate, resolve.
- Fuse the images along the seamline with Poisson fusion

## Seamline path optimization

Once the full seamline is found there may be some visual defects. These can be dealt with by optimizing the path of the pixels on the seamline that have a poor evaluation score. The average SDSIM and average MAE of the entire seamline are first calculated. Then the pixels on the seamline are evaluated one at a time, starting at the starting pixel of the original seamline. Each pixel's indicators (SDSIM and MAE) are averaged with its seamline neighbor's indicators. The evaluation proceeds until a pixel is found whose SDSIM $\times$ MAE product is greater than or equal to the product of the average SDSIM and average MAE for the entire seam. When a pixel is found that meets this criteria the seamline is truncated at that pixel and a new seamline is recalculated from the remaining section. The feature maps and weight cost map are recalculated for the truncated area, then FMM runs again from the truncated pixel to the original end point. 

