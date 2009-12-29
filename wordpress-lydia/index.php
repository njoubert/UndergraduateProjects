<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">

<html lang="en">
<head>
	<meta http-equiv="Content-Type" content="<?php bloginfo('html_type'); ?>; charset=<?php bloginfo('charset'); ?>" />
	<title><?php wp_title('&laquo;', true, 'right'); ?> <?php bloginfo('name'); ?></title>	
	<meta name="author" content="Niels Joubert">
	
	<!-- Date: 2009-12-28 -->
	<link rel="stylesheet" href="<?php bloginfo('stylesheet_directory'); ?>/css/blueprint/screen.css" type="text/css" media="screen, projection">
	<link rel="stylesheet" href="<?php bloginfo('stylesheet_directory'); ?>/css/blueprint/print.css" type="text/css" media="print">
	<!--[if lt IE 8]>
	  <link rel="stylesheet" href="<?php bloginfo('stylesheet_directory'); ?>/css/blueprint/ie.css" type="text/css" media="screen, projection">
	<![endif]-->
	<link rel="stylesheet" href="<?php bloginfo('stylesheet_url'); ?>" type="text/css" media="screen" />
	<link rel="pingback" href="<?php bloginfo('pingback_url'); ?>" />

	<script language="JavaScript">
	        function printPage() { print(document); }
	</script>

	<?php wp_head(); ?>
<style>
.header {
	background-image:url('<?php bloginfo('stylesheet_directory'); ?>/images/Heading-1.jpg');
}
</style>
</head>

<body <?php body_class(); ?>>
	<div class="container">
  	<div class="span-24 header top">
  		
  	</div>
		<div class="span-24 navbar">
			<ul>
			<?php wp_list_pages('title_li=&sort_column=menu_order&depth=1'); ?>
     			</ul>
		</div>

		<div class="content">
			<div id="posts">
	<?php if (have_posts()) : ?>

		<?php while (have_posts()) : the_post(); ?>

			<div <?php post_class() ?> id="post-<?php the_ID(); ?>">
				<span class="pagetitle"><h1><a href="<?php the_permalink() ?>" rel="bookmark" title="Permanent Link to <?php the_title_attribute(); ?>"><?php the_title(); ?></a></h1></span>
				<!-- <small><?php the_time('F jS, Y')
				?> </small> --> 

				<div class="entry">
					<?php the_content('Read the rest of this entry &raquo;'); ?>
				</div>

			</div>

		<?php endwhile; ?>

		<div class="navigation">
			<div class="alignleft"><?php next_posts_link('&laquo; Older Entries') ?></div>
			<div class="alignright"><?php previous_posts_link('Newer Entries &raquo;') ?></div>
		</div>

	<?php else : ?>

		<h2 class="center">Not Found</h2>
		<p class="center">Sorry, but you are looking for something that isn't here.</p>
		<?php get_search_form(); ?>

	<?php endif; ?>
</div>

		</div>
		
  	<div class="span-24 footer bottom">
			Header image: Biofilm of
			    a bacterium (Pseudomonas) and a yeast (Cryptococcus) stained with Calcofluor
			    White and FUN-1, ink-outlined.<br/>

			        <?php bloginfo('name'); ?> is powered by
			        <a href="http://wordpress.org/">WordPress</a>. Subscribe
			        to the <a href="<?php bloginfo('rss2_url');
			        ?>">Entries (RSS)</a>
			        or <a href="<?php bloginfo('comments_rss2_url');
			        ?>">Comments (RSS)</a>
			        feed. <?php wp_register('',', '); ?><?php wp_loginout(); ?>
			        <!-- <?php echo get_num_queries(); ?> queries. <?php timer_stop(1); ?> seconds. -->

  	</div>
  </div>

	<?php wp_footer(); ?>

<script type="text/javascript">
var gaJsHost = (("https:" == document.location.protocol) ? "https://ssl." : "http://www.");
document.write(unescape("%3Cscript src='" + gaJsHost + "google-analytics.com/ga.js' type='text/javascript'%3E%3C/script%3E"));
</script>
<script type="text/javascript">
try {
var pageTracker = _gat._getTracker("UA-12235406-1");
pageTracker._trackPageview();
} catch(err) {}</script>
</body>
</html>
